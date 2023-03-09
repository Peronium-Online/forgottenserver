cmake_minimum_required(VERSION 3.14)

# Set the project name and version
project(peronium_server_test VERSION 1.0 LANGUAGES CXX)

# Locate GTest package
find_package(GTest CONFIG REQUIRED)

# Add the executable for the tests
add_executable(peronium_server_test
    ${peronium_SRC}
    ${CMAKE_CURRENT_LIST_DIR}/unit/creature_team_test.cpp
)

# Include the headers for the tests
target_include_directories(peronium_server_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

# Link the libraries for the tests
include_directories(${Boost_INCLUDE_DIRS} ${Crypto++_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${MYSQL_INCLUDE_DIR} ${PUGIXML_INCLUDE_DIR})
target_link_libraries(peronium_server_test PRIVATE
    GTest::gmock
    GTest::gtest
    GTest::gmock_main
    GTest::gtest_main
    Boost::date_time
    Boost::system
    Boost::iostreams
    fmt::fmt
    ${CMAKE_THREAD_LIBS_INIT}
    ${Crypto++_LIBRARIES}
    ${LUA_LIBRARIES}
    ${MYSQL_CLIENT_LIBS}
    ${PUGIXML_LIBRARIES}
)

# Define the C++ standard for the project
set_target_properties(peronium_server_test PROPERTIES CXX_STANDARD 17)
set_target_properties(peronium_server_test PROPERTIES CXX_STANDARD_REQUIRED ON)

# Add a custom target for running the tests
add_custom_target(run-tests
    COMMAND peronium_server_test
    DEPENDS peronium_server_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Define the installation rules
install(TARGETS peronium_server_test
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

if (${CMAKE_VERSION} VERSION_GREATER "3.16.0")
    target_precompile_headers(peronium_server_test PUBLIC ../src/otpch.h)
else ()
    include(cotire)
    set_target_properties(peronium_server_test PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "../src/otpch.h")
    set_target_properties(peronium_server_test PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    cotire(peronium_server_test)
endif ()
