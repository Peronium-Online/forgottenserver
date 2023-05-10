#include "libs/monster/Monster.h"

#include "game.h"
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
