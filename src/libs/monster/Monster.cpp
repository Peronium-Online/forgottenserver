#include "libs/monster/Monster.h"

#include "../../game.h"
#include "../../position.h"
#include "libs/util/tools/direction.h"
#include "libs/util/tools/random.h"
#include "spectators.h"

extern Monsters g_monsters;
extern Game g_game;

Monster* Monster::createMonsterByName(const std::string& name)
{
	MonsterType* mType = g_monsters.findMonsterTypeByName(name);
	if (!mType) {
		return nullptr;
	}

	return new Monster(mType);
}

const std::string& Monster::getName() const
{
	if (name.empty()) {
		return mType->name;
	}

	return name;
}

std::string Monster::getDescription(int32_t) const { return this->mType->nameDescription + '.'; }

const std::string& Monster::getNameDescription() const
{
	if (nameDescription.empty()) {
		return mType->nameDescription;
	}
	return nameDescription;
}

void Monster::setID()
{
	if (id == 0) {
		id = MONSTER_AUTO_ID++;
	}
}

void Monster::addList() { g_game.addMonster(this); }

void Monster::removeList() { g_game.removeMonster(this); }

void Monster::setName(const std::string& name)
{
	if (getName() == name) {
		return;
	}

	this->name = name;

	// NOTE: Due to how client caches known creatures, it is not feasible to send creature update to everyone that has
	// ever met it
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, position, true, true);
	for (Creature* spectator : spectators) {
		assert(dynamic_cast<Player*>(spectator) != nullptr);
		static_cast<Player*>(spectator)->sendUpdateTileCreature(this);
	}
}

bool Monster::isInSpawnRange(const Position& pos) const
{
	if (!spawn) {
		return true;
	}

	if (Monster::despawnRadius == 0) {
		return true;
	}

	if (!Spawns::isInZone(masterPos, Monster::despawnRadius, pos)) {
		return false;
	}

	if (Monster::despawnRange == 0) {
		return true;
	}

	if (Position::getDistanceZ(pos, masterPos) > Monster::despawnRange) {
		return false;
	}

	return true;
}

void Monster::clearTargetList()
{
	for (Creature* creature : targetList) {
		creature->decrementReferenceCounter();
	}
	targetList.clear();
}

void Monster::clearFriendList()
{
	for (Creature* creature : friendList) {
		creature->decrementReferenceCounter();
	}
	friendList.clear();
}

void Monster::setIdle(bool idle)
{
	if (isRemoved() || isDead()) {
		return;
	}

	this->idle = idle;

	if (!isIdle) {
		g_game.addCreatureCheck(this);
	} else {
		onIdleStatus();
		clearTargetList();
		clearFriendList();
		Game::removeCreatureCheck(this);
	}
}

void Monster::updateIdleStatus()
{
	bool idle = false;
	if (!isSummon() && targetList.empty()) {
		// check if there are aggressive conditions
		idle = std::find_if(conditions.begin(), conditions.end(),
		                    [](Condition* condition) { return condition->isAggressive(); }) == conditions.end();
	}

	setIdle(idle);
}

bool Monster::isTarget(const Creature* creature) const
{
	if ((creature->isRemoved() || !creature->isAttackable() || creature->getZone() == ZONE_PROTECTION ||
	     !canSeeCreature(creature)) &&
	    !isSeekTarget(creature)) {
		return false;
	}

	if (creature->getPosition().z != getPosition().z) {
		return false;
	}
	return true;
}

bool Monster::isSeekTarget(const Creature* creature) const
{
	if (creature == nullptr || creature->isDead()) {
		return false;
	}

	auto lowerCasedName = boost::algorithm::to_lower_copy(creature->getName());
	if (mType->info.seeks.size() > 0) {
		for (auto target : mType->info.seeks) {
			auto lowerCasedTarget = boost::algorithm::to_lower_copy(target.getName());
			if (lowerCasedName.compare(lowerCasedTarget) == 0) {
				return true;
			}
		}
	}

	return false;
}

bool Monster::isOpponent(const Creature* creature) const
{
	if (isSeekTarget(creature)) {
		return true;
	}

	if (isSummon() && getMaster()->getPlayer()) {
		if (creature != getMaster()) {
			return true;
		}
	} else {
		if ((creature->getPlayer() && !creature->getPlayer()->hasFlag(PlayerFlag_IgnoredByMonsters)) ||
		    (creature->getMaster() && creature->getMaster()->getPlayer())) {
			return true;
		}
	}

	return false;
}

bool Monster::isFriend(const Creature* creature) const
{
	if (isSummon() && getMaster()->getPlayer()) {
		const Player* masterPlayer = getMaster()->getPlayer();
		const Player* tmpPlayer = nullptr;

		if (creature->getPlayer()) {
			tmpPlayer = creature->getPlayer();
		} else {
			const Creature* creatureMaster = creature->getMaster();

			if (creatureMaster && creatureMaster->getPlayer()) {
				tmpPlayer = creatureMaster->getPlayer();
			}
		}

		if (tmpPlayer && (tmpPlayer == getMaster() || masterPlayer->isPartner(tmpPlayer))) {
			return true;
		}
	} else if (creature->getMonster() && !creature->isSummon() && !isSeekTarget(creature)) {
		return true;
	}

	return false;
}

void Monster::addFriend(Creature* creature)
{
	assert(creature != this);
	auto result = friendList.insert(creature);
	if (result.second) {
		creature->incrementReferenceCounter();
	}
}

void Monster::removeFriend(Creature* creature)
{
	auto it = friendList.find(creature);
	if (it != friendList.end()) {
		creature->decrementReferenceCounter();
		friendList.erase(it);
	}
}

void Monster::addTarget(Creature* creature, bool pushFront /* = false*/)
{
	assert(creature != this);
	if (std::find(targetList.begin(), targetList.end(), creature) == targetList.end()) {
		creature->incrementReferenceCounter();
		if (pushFront) {
			targetList.push_front(creature);
		} else {
			targetList.push_back(creature);
		}
	}
}

void Monster::removeTarget(Creature* creature)
{
	auto it = std::find(targetList.begin(), targetList.end(), creature);
	if (it != targetList.end()) {
		creature->decrementReferenceCounter();
		targetList.erase(it);
	}
}

bool Monster::searchTarget(TargetSearchType_t searchType /*= TARGETSEARCH_DEFAULT*/)
{
	std::list<Creature*> resultList;
	const Position& myPos = getPosition();

	for (Creature* creature : targetList) {
		if (followCreature != creature && isTarget(creature)) {
			if (searchType == TARGETSEARCH_RANDOM || canUseAttack(myPos, creature)) {
				resultList.push_back(creature);
			}
		}
	}

	switch (searchType) {
		case TARGETSEARCH_NEAREST: {
			Creature* target = nullptr;
			if (!resultList.empty()) {
				auto it = resultList.begin();
				target = *it;

				if (++it != resultList.end()) {
					const Position& targetPosition = target->getPosition();
					int32_t minRange =
					    Position::getDistanceX(myPos, targetPosition) + Position::getDistanceY(myPos, targetPosition);
					do {
						const Position& pos = (*it)->getPosition();

						int32_t distance = Position::getDistanceX(myPos, pos) + Position::getDistanceY(myPos, pos);
						if (distance < minRange) {
							target = *it;
							minRange = distance;
						}
					} while (++it != resultList.end());
				}
			} else {
				int32_t minRange = std::numeric_limits<int32_t>::max();
				for (Creature* creature : targetList) {
					if (!isTarget(creature)) {
						continue;
					}

					const Position& pos = creature->getPosition();
					int32_t distance = Position::getDistanceX(myPos, pos) + Position::getDistanceY(myPos, pos);
					if (distance < minRange) {
						target = creature;
						minRange = distance;
					}
				}
			}

			if (target && selectTarget(target)) {
				return true;
			}
			break;
		}

		case TARGETSEARCH_DEFAULT:
		case TARGETSEARCH_ATTACKRANGE:
		case TARGETSEARCH_RANDOM:
		default: {
			if (!resultList.empty()) {
				auto it = resultList.begin();
				std::advance(it, uniform_random(0, resultList.size() - 1));
				return selectTarget(*it);
			}

			if (searchType == TARGETSEARCH_ATTACKRANGE) {
				return false;
			}

			break;
		}
	}

	// lets just pick the first target in the list
	for (Creature* target : targetList) {
		if (followCreature != target && selectTarget(target)) {
			return true;
		}
	}
	return false;
}

bool Monster::selectTarget(Creature* creature)
{
	if (!isTarget(creature)) {
		return false;
	}

	auto it = std::find(targetList.begin(), targetList.end(), creature);
	if (it == targetList.end()) {
		// Target not found in our target list.
		return false;
	}

	if (isHostile() || isSummon()) {
		if (setAttackedCreature(creature) && !isSummon()) {
			g_dispatcher.addTask([id = getID()]() { g_game.checkCreatureAttack(id); });
		}
	}
	return setFollowCreature(creature);
}

bool Monster::canUseAttack(const Position& pos, const Creature* target) const
{
	if (isHostile()) {
		const Position& targetPos = target->getPosition();
		uint32_t distance =
		    std::max<uint32_t>(Position::getDistanceX(pos, targetPos), Position::getDistanceY(pos, targetPos));
		for (const MonsterSpell& spellBlock : mType->info.attackSpells) {
			if (spellBlock.range != 0 && distance <= spellBlock.range) {
				return g_game.isSightClear(pos, targetPos, true);
			}
		}
		return false;
	}
	return true;
}

bool Monster::walkToSpawn()
{
	if (walkingToSpawn || !spawn || !targetList.empty()) {
		return false;
	}

	int32_t distance =
	    std::max<int32_t>(Position::getDistanceX(position, masterPos), Position::getDistanceY(position, masterPos));
	if (distance == 0) {
		return false;
	}

	listWalkDir.clear();
	if (!getPathTo(masterPos, listWalkDir, 0, std::max<int32_t>(0, distance - 5), true, true, distance)) {
		return false;
	}

	walkingToSpawn = true;
	startAutoWalk();
	return true;
}

bool Monster::getDistanceStep(const Position& targetPos, Direction& direction, bool flee /* = false */)
{
	const Position& creaturePos = getPosition();

	int_fast32_t dx = Position::getDistanceX(creaturePos, targetPos);
	int_fast32_t dy = Position::getDistanceY(creaturePos, targetPos);

	int32_t distance = std::max<int32_t>(dx, dy);

	if (!flee && (distance > mType->info.targetDistance || !g_game.isSightClear(creaturePos, targetPos, true))) {
		return false; // let the A* calculate it
	} else if (!flee && distance == mType->info.targetDistance) {
		return true; // we don't really care here, since it's what we wanted to reach (a dance-step will take of dancing
		             // in that position)
	}

	int_fast32_t offsetx = Position::getOffsetX(creaturePos, targetPos);
	int_fast32_t offsety = Position::getOffsetY(creaturePos, targetPos);

	if (dx <= 1 && dy <= 1) {
		// seems like a target is near, it this case we need to slow down our movements (as a monster)
		if (stepDuration < 2) {
			stepDuration++;
		}
	} else if (stepDuration > 0) {
		stepDuration--;
	}

	if (offsetx == 0 && offsety == 0) {
		return getRandomStep(
		    creaturePos,
		    direction); // player is "on" the monster so let's get some random step and rest will be taken care later.
	}

	if (dx == dy) {
		// player is diagonal to the monster
		if (offsetx >= 1 && offsety >= 1) {
			// player is NW
			// escape to SE, S or E [and some extra]
			bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
			bool e = canWalkTo(creaturePos, DIRECTION_EAST);

			if (s && e) {
				direction = boolean_random() ? DIRECTION_SOUTH : DIRECTION_EAST;
				return true;
			} else if (s) {
				direction = DIRECTION_SOUTH;
				return true;
			} else if (e) {
				direction = DIRECTION_EAST;
				return true;
			} else if (canWalkTo(creaturePos, DIRECTION_SOUTHEAST)) {
				direction = DIRECTION_SOUTHEAST;
				return true;
			}

			/* fleeing */
			bool n = canWalkTo(creaturePos, DIRECTION_NORTH);
			bool w = canWalkTo(creaturePos, DIRECTION_WEST);

			if (flee) {
				if (n && w) {
					direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_WEST;
					return true;
				} else if (n) {
					direction = DIRECTION_NORTH;
					return true;
				} else if (w) {
					direction = DIRECTION_WEST;
					return true;
				}
			}

			/* end of fleeing */

			if (w && canWalkTo(creaturePos, DIRECTION_SOUTHWEST)) {
				direction = DIRECTION_WEST;
			} else if (n && canWalkTo(creaturePos, DIRECTION_NORTHEAST)) {
				direction = DIRECTION_NORTH;
			}

			return true;
		} else if (offsetx <= -1 && offsety <= -1) {
			// player is SE
			// escape to NW , W or N [and some extra]
			bool w = canWalkTo(creaturePos, DIRECTION_WEST);
			bool n = canWalkTo(creaturePos, DIRECTION_NORTH);

			if (w && n) {
				direction = boolean_random() ? DIRECTION_WEST : DIRECTION_NORTH;
				return true;
			} else if (w) {
				direction = DIRECTION_WEST;
				return true;
			} else if (n) {
				direction = DIRECTION_NORTH;
				return true;
			}

			if (canWalkTo(creaturePos, DIRECTION_NORTHWEST)) {
				direction = DIRECTION_NORTHWEST;
				return true;
			}

			/* fleeing */
			bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
			bool e = canWalkTo(creaturePos, DIRECTION_EAST);

			if (flee) {
				if (s && e) {
					direction = boolean_random() ? DIRECTION_SOUTH : DIRECTION_EAST;
					return true;
				} else if (s) {
					direction = DIRECTION_SOUTH;
					return true;
				} else if (e) {
					direction = DIRECTION_EAST;
					return true;
				}
			}

			/* end of fleeing */

			if (s && canWalkTo(creaturePos, DIRECTION_SOUTHWEST)) {
				direction = DIRECTION_SOUTH;
			} else if (e && canWalkTo(creaturePos, DIRECTION_NORTHEAST)) {
				direction = DIRECTION_EAST;
			}

			return true;
		} else if (offsetx >= 1 && offsety <= -1) {
			// player is SW
			// escape to NE, N, E [and some extra]
			bool n = canWalkTo(creaturePos, DIRECTION_NORTH);
			bool e = canWalkTo(creaturePos, DIRECTION_EAST);
			if (n && e) {
				direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_EAST;
				return true;
			} else if (n) {
				direction = DIRECTION_NORTH;
				return true;
			} else if (e) {
				direction = DIRECTION_EAST;
				return true;
			}

			if (canWalkTo(creaturePos, DIRECTION_NORTHEAST)) {
				direction = DIRECTION_NORTHEAST;
				return true;
			}

			/* fleeing */
			bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
			bool w = canWalkTo(creaturePos, DIRECTION_WEST);

			if (flee) {
				if (s && w) {
					direction = boolean_random() ? DIRECTION_SOUTH : DIRECTION_WEST;
					return true;
				} else if (s) {
					direction = DIRECTION_SOUTH;
					return true;
				} else if (w) {
					direction = DIRECTION_WEST;
					return true;
				}
			}

			/* end of fleeing */

			if (w && canWalkTo(creaturePos, DIRECTION_NORTHWEST)) {
				direction = DIRECTION_WEST;
			} else if (s && canWalkTo(creaturePos, DIRECTION_SOUTHEAST)) {
				direction = DIRECTION_SOUTH;
			}

			return true;
		} else if (offsetx <= -1 && offsety >= 1) {
			// player is NE
			// escape to SW, S, W [and some extra]
			bool w = canWalkTo(creaturePos, DIRECTION_WEST);
			bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
			if (w && s) {
				direction = boolean_random() ? DIRECTION_WEST : DIRECTION_SOUTH;
				return true;
			} else if (w) {
				direction = DIRECTION_WEST;
				return true;
			} else if (s) {
				direction = DIRECTION_SOUTH;
				return true;
			} else if (canWalkTo(creaturePos, DIRECTION_SOUTHWEST)) {
				direction = DIRECTION_SOUTHWEST;
				return true;
			}

			/* fleeing */
			bool n = canWalkTo(creaturePos, DIRECTION_NORTH);
			bool e = canWalkTo(creaturePos, DIRECTION_EAST);

			if (flee) {
				if (n && e) {
					direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_EAST;
					return true;
				} else if (n) {
					direction = DIRECTION_NORTH;
					return true;
				} else if (e) {
					direction = DIRECTION_EAST;
					return true;
				}
			}

			/* end of fleeing */

			if (e && canWalkTo(creaturePos, DIRECTION_SOUTHEAST)) {
				direction = DIRECTION_EAST;
			} else if (n && canWalkTo(creaturePos, DIRECTION_NORTHWEST)) {
				direction = DIRECTION_NORTH;
			}

			return true;
		}
	}

	// Now let's decide where the player is located to the monster (what direction) so we can decide where to escape.
	if (dy > dx) {
		Direction playerDir = offsety < 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;
		switch (playerDir) {
			case DIRECTION_NORTH: {
				// Player is to the NORTH, so obviously we need to check if we can go SOUTH, if not then let's choose
				// WEST or EAST and again if we can't we need to decide about some diagonal movements.
				if (canWalkTo(creaturePos, DIRECTION_SOUTH)) {
					direction = DIRECTION_SOUTH;
					return true;
				}

				bool w = canWalkTo(creaturePos, DIRECTION_WEST);
				bool e = canWalkTo(creaturePos, DIRECTION_EAST);
				if (w && e && offsetx == 0) {
					direction = boolean_random() ? DIRECTION_WEST : DIRECTION_EAST;
					return true;
				} else if (w && offsetx <= 0) {
					direction = DIRECTION_WEST;
					return true;
				} else if (e && offsetx >= 0) {
					direction = DIRECTION_EAST;
					return true;
				}

				/* fleeing */
				if (flee) {
					if (w && e) {
						direction = boolean_random() ? DIRECTION_WEST : DIRECTION_EAST;
						return true;
					} else if (w) {
						direction = DIRECTION_WEST;
						return true;
					} else if (e) {
						direction = DIRECTION_EAST;
						return true;
					}
				}

				/* end of fleeing */

				bool sw = canWalkTo(creaturePos, DIRECTION_SOUTHWEST);
				bool se = canWalkTo(creaturePos, DIRECTION_SOUTHEAST);
				if (sw || se) {
					// we can move both dirs
					if (sw && se) {
						direction = boolean_random() ? DIRECTION_SOUTHWEST : DIRECTION_SOUTHEAST;
					} else if (w) {
						direction = DIRECTION_WEST;
					} else if (sw) {
						direction = DIRECTION_SOUTHWEST;
					} else if (e) {
						direction = DIRECTION_EAST;
					} else if (se) {
						direction = DIRECTION_SOUTHEAST;
					}
					return true;
				}

				/* fleeing */
				if (flee && canWalkTo(creaturePos, DIRECTION_NORTH)) {
					// towards player, yea
					direction = DIRECTION_NORTH;
					return true;
				}

				/* end of fleeing */
				break;
			}

			case DIRECTION_SOUTH: {
				if (canWalkTo(creaturePos, DIRECTION_NORTH)) {
					direction = DIRECTION_NORTH;
					return true;
				}

				bool w = canWalkTo(creaturePos, DIRECTION_WEST);
				bool e = canWalkTo(creaturePos, DIRECTION_EAST);
				if (w && e && offsetx == 0) {
					direction = boolean_random() ? DIRECTION_WEST : DIRECTION_EAST;
					return true;
				} else if (w && offsetx <= 0) {
					direction = DIRECTION_WEST;
					return true;
				} else if (e && offsetx >= 0) {
					direction = DIRECTION_EAST;
					return true;
				}

				/* fleeing */
				if (flee) {
					if (w && e) {
						direction = boolean_random() ? DIRECTION_WEST : DIRECTION_EAST;
						return true;
					} else if (w) {
						direction = DIRECTION_WEST;
						return true;
					} else if (e) {
						direction = DIRECTION_EAST;
						return true;
					}
				}

				/* end of fleeing */

				bool nw = canWalkTo(creaturePos, DIRECTION_NORTHWEST);
				bool ne = canWalkTo(creaturePos, DIRECTION_NORTHEAST);
				if (nw || ne) {
					// we can move both dirs
					if (nw && ne) {
						direction = boolean_random() ? DIRECTION_NORTHWEST : DIRECTION_NORTHEAST;
					} else if (w) {
						direction = DIRECTION_WEST;
					} else if (nw) {
						direction = DIRECTION_NORTHWEST;
					} else if (e) {
						direction = DIRECTION_EAST;
					} else if (ne) {
						direction = DIRECTION_NORTHEAST;
					}
					return true;
				}

				/* fleeing */
				if (flee && canWalkTo(creaturePos, DIRECTION_SOUTH)) {
					// towards player, yea
					direction = DIRECTION_SOUTH;
					return true;
				}

				/* end of fleeing */
				break;
			}

			default:
				break;
		}
	} else {
		Direction playerDir = offsetx < 0 ? DIRECTION_EAST : DIRECTION_WEST;
		switch (playerDir) {
			case DIRECTION_WEST: {
				if (canWalkTo(creaturePos, DIRECTION_EAST)) {
					direction = DIRECTION_EAST;
					return true;
				}

				bool n = canWalkTo(creaturePos, DIRECTION_NORTH);
				bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
				if (n && s && offsety == 0) {
					direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_SOUTH;
					return true;
				} else if (n && offsety <= 0) {
					direction = DIRECTION_NORTH;
					return true;
				} else if (s && offsety >= 0) {
					direction = DIRECTION_SOUTH;
					return true;
				}

				/* fleeing */
				if (flee) {
					if (n && s) {
						direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_SOUTH;
						return true;
					} else if (n) {
						direction = DIRECTION_NORTH;
						return true;
					} else if (s) {
						direction = DIRECTION_SOUTH;
						return true;
					}
				}

				/* end of fleeing */

				bool se = canWalkTo(creaturePos, DIRECTION_SOUTHEAST);
				bool ne = canWalkTo(creaturePos, DIRECTION_NORTHEAST);
				if (se || ne) {
					if (se && ne) {
						direction = boolean_random() ? DIRECTION_SOUTHEAST : DIRECTION_NORTHEAST;
					} else if (s) {
						direction = DIRECTION_SOUTH;
					} else if (se) {
						direction = DIRECTION_SOUTHEAST;
					} else if (n) {
						direction = DIRECTION_NORTH;
					} else if (ne) {
						direction = DIRECTION_NORTHEAST;
					}
					return true;
				}

				/* fleeing */
				if (flee && canWalkTo(creaturePos, DIRECTION_WEST)) {
					// towards player, yea
					direction = DIRECTION_WEST;
					return true;
				}

				/* end of fleeing */
				break;
			}

			case DIRECTION_EAST: {
				if (canWalkTo(creaturePos, DIRECTION_WEST)) {
					direction = DIRECTION_WEST;
					return true;
				}

				bool n = canWalkTo(creaturePos, DIRECTION_NORTH);
				bool s = canWalkTo(creaturePos, DIRECTION_SOUTH);
				if (n && s && offsety == 0) {
					direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_SOUTH;
					return true;
				} else if (n && offsety <= 0) {
					direction = DIRECTION_NORTH;
					return true;
				} else if (s && offsety >= 0) {
					direction = DIRECTION_SOUTH;
					return true;
				}

				/* fleeing */
				if (flee) {
					if (n && s) {
						direction = boolean_random() ? DIRECTION_NORTH : DIRECTION_SOUTH;
						return true;
					} else if (n) {
						direction = DIRECTION_NORTH;
						return true;
					} else if (s) {
						direction = DIRECTION_SOUTH;
						return true;
					}
				}

				/* end of fleeing */

				bool nw = canWalkTo(creaturePos, DIRECTION_NORTHWEST);
				bool sw = canWalkTo(creaturePos, DIRECTION_SOUTHWEST);
				if (nw || sw) {
					if (nw && sw) {
						direction = boolean_random() ? DIRECTION_NORTHWEST : DIRECTION_SOUTHWEST;
					} else if (n) {
						direction = DIRECTION_NORTH;
					} else if (nw) {
						direction = DIRECTION_NORTHWEST;
					} else if (s) {
						direction = DIRECTION_SOUTH;
					} else if (sw) {
						direction = DIRECTION_SOUTHWEST;
					}
					return true;
				}

				/* fleeing */
				if (flee && canWalkTo(creaturePos, DIRECTION_EAST)) {
					// towards player, yea
					direction = DIRECTION_EAST;
					return true;
				}

				/* end of fleeing */
				break;
			}

			default:
				break;
		}
	}

	return true;
}

bool Monster::getRandomStep(const Position& creaturePos, Direction& direction) const
{
	static std::vector<Direction> dirList{DIRECTION_NORTH, DIRECTION_WEST, DIRECTION_EAST, DIRECTION_SOUTH};
	std::shuffle(dirList.begin(), dirList.end(), getRandomGenerator());

	for (Direction dir : dirList) {
		if (canWalkTo(creaturePos, dir)) {
			direction = dir;
			return true;
		}
	}
	return false;
}

bool Monster::canWalkTo(Position pos, Direction direction) const
{
	pos = getNextPosition(direction, pos);
	if (isInSpawnRange(pos)) {
		if (getWalkCache(pos) == 0) {
			return false;
		}

		Tile* tile = g_game.map.getTile(pos);
		if (tile && !tile->getTopVisibleCreature(this) &&
		    tile->queryAdd(0, *this, 1, FLAG_PATHFINDING) == RETURNVALUE_NOERROR) {
			return true;
		}
	}
	return false;
}

bool Monster::canWalkOnFieldType(CombatType_t combatType) const
{
	switch (combatType) {
		case COMBAT_ENERGYDAMAGE:
			return mType->info.canWalkOnEnergy;
		case COMBAT_FIREDAMAGE:
			return mType->info.canWalkOnFire;
		case COMBAT_EARTHDAMAGE:
			return mType->info.canWalkOnPoison;
		default:
			return true;
	}
}
