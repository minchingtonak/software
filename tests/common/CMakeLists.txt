project(tests)
enable_testing()
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories(
        ${SW_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR}
)

set(TEST_SRCS
        MathTest.cpp)

foreach (testSrc ${TEST_SRCS})
    #Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    #Add compile target
    add_executable(${testName} ${testSrc})

    #link to Boost libraries AND your targets and dependencies
    target_link_libraries(${testName} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

    set(TEST_BIN_DIR ${CMAKE_SOURCE_DIR}/bin/test)

    #I like to move testing binaries into a testBin directory
    set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})

    #Finally add it to test execution -
    #Notice the WORKING_DIRECTORY and COMMAND
    add_test(NAME ${testName}
            WORKING_DIRECTORY ${TEST_BIN_DIR}
            COMMAND ${TEST_BIN_DIR}/${testName})
endforeach (testSrc)
