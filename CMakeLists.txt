#### CMakeLists.txt : CMake configuration and build script
####                  for the gtDMMB/pmfe sources
#### Author: Maxie D. Schmidt (github.com/maxieds)
#### Created: 2020.11.02

## CMake specific configuration and build environment settings:
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_LINKER g++)
set(SHELL /bin/bash)
set(CMAKE_RULE_MESSAGES ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

## Define the PMFE project and define the output binaries:
project(gtDMMB_PMFE)

set(PMFE_VERSION_MAJOR "1")
set(PMFE_VERSION_MINOR "0")
set(PMFE_VERSION_PATCH "0")
set(VERSION "${PMFE_VERSION_MAJOR}.${PMFE_VERSION_MINOR}.${PMFE_VERSION_PATCH}")
set(PMFE_PACKAGE_NAME "gtDMMB-PMFE")
set(PMFE_PACKAGE_VERSION ${VERSION})
set(PMFE_PACKAGE_STRING "${PMFE_PACKAGE_NAME} v${PMFE_PACKAGE_VERSION}")

## Option to enable auto running of unit tests after recompiling:
option(RUN_UNIT_TESTS "Enable running tests after build" OFF)

## Configure pkg-config script for some dependency libraries:
include(FindPkgConfig)
if(NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Unable to configure libGMP : pkg-config not found ..." )
endif()

if(APPLE)
     execute_process(
          COMMAND bash -c "echo -n $(greadlink -f $(find .. -mindepth 1 -maxdepth 1 -iname 'gmp-*' -type d -printf \"../%f\" | head -n1))"
          OUTPUT_VARIABLE PkgConfigLocalPCFilesPath
     )
elseif(UNIX AND NOT APPLE)
     execute_process(
          COMMAND bash -c "echo -n $(readlink -f $(find .. -mindepth 1 -maxdepth 1 -iname 'gmp-*' -type d -printf \"../%f\" | head -n1))"
          OUTPUT_VARIABLE PkgConfigLocalPCFilesPath
     )
endif()
string(STRIP "${PkgConfigLocalPCFilesPath}" PkgConfigLocalPCFilesPath)

set(ENV{PKG_CONFIG_PATH} "${PkgConfigLocalPCFilesPath}")
execute_process(
     COMMAND bash -c "pkg-config gmp --libs --static"
     OUTPUT_VARIABLE PC_GMP_LIBRARIES
)
string(STRIP "${PC_GMP_LIBRARIES}" PC_GMP_LIBRARIES)
execute_process(
     COMMAND bash -c "pkg-config gmp --cflags"
     OUTPUT_VARIABLE PC_GMP_INCLUDE_DIRS
)
string(STRIP "${PC_GMP_INCLUDE_DIRS}" PC_GMP_INCLUDE_DIRS)
execute_process(
     COMMAND bash -c "pkg-config gmpxx --libs --static"
     OUTPUT_VARIABLE PC_GMPXX_LIBRARIES
)
string(STRIP "${PC_GMPXX_LIBRARIES}" PC_GMPXX_LIBRARIES)
execute_process(
     COMMAND bash -c "pkg-config gmpxx --cflags"
     OUTPUT_VARIABLE PC_GMPXX_INCLUDE_DIRS
)
string(STRIP "${PC_GMPXX_INCLUDE_DIRS}" PC_GMPXX_INCLUDE_DIRS)

## Configure paths on requisite build time libraries (Boost):
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_FIND_REQUIRED TRUE)
set(Boost_FIND_QUIETLY TRUE)
set(Boost_DEBUG FALSE)
set(Boost_NO_BOOST_CMAKE TRUE)
set(Boost_NO_SYSTEM_PATHS TRUE)
if(NOT DEFINED BOOST_ROOT)
     set(BoostFileSystemRootPath "../BoostLocalInstall")
     set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/${BoostFileSystemRootPath})
endif()
set(Boost_INCLUDE_DIR ${BOOST_ROOT}/include)
set(Boost_LIBRARY_DIR ${BOOST_ROOT}/lib)
#find_package(
#     Boost ${BOOST_MIN_VERSION}
#     COMPONENTS filesystem program_options system log log_setup thread atomic regex chrono
#     REQUIRED
#)
#if(NOT Boost_FOUND)
#    message(FATAL_ERROR "Boost Not Found ... Unable to complete the build")
#endif()

## Configure paths on requisite build time libraries (CGAL):
if(APPLE)
     execute_process(
          COMMAND bash "-c" "echo $(greadlink -f $(find .. -mindepth 1 -maxdepth 1 -iname 'cgal*' -type d -printf \"${PROJECT_SOURCE_DIR}/../%f\n\" | head -n1))"
          OUTPUT_VARIABLE CGALFileSystemRootPath
          RESULT_VARIABLE CGALFileSystemRootPath_CmdResult
     )
     string(STRIP ${CGALFileSystemRootPath} CGALFileSystemRootPath)
     set(CGAL_DIR ${CGALFileSystemRootPath})
elseif(UNIX AND NOT APPLE)
     execute_process(
          COMMAND bash "-c" "echo $(readlink -f $(find .. -mindepth 1 -maxdepth 1 -iname 'cgal*' -type d -printf \"${PROJECT_SOURCE_DIR}/../%f\n\" | head -n1))"
          OUTPUT_VARIABLE CGALFileSystemRootPath
          RESULT_VARIABLE CGALFileSystemRootPath_CmdResult
     )
     string(STRIP ${CGALFileSystemRootPath} CGALFileSystemRootPath)
     set(CGAL_DIR ${CGALFileSystemRootPath})
endif()
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "Force CGAL to maintain CMAKE flags")
file(GLOB CGALBaseFilesList "${CGAL_DIR}/*")
set(CGALIncludeFilesList "")
foreach(fsEntry ${CGALBaseFilesList})
     if(NOT ${fsEntry} MATCHES ".*\\.svn\$" AND IS_DIRECTORY ${fsEntry}
        AND IS_DIRECTORY ${fsEntry}/include)
          list(APPEND CGALIncludeFilesList "${fsEntry}/include")
     endif()
endforeach()
# AND NOT EXISTS ${fsEntry}/include/CGAL/Regular_complex_d.h

## Build the binaries for the PMFE project:

add_custom_command(
    OUTPUT BuildConfigHeaderIsSetup
    COMMAND cmake "-P" "${PROJECT_SOURCE_DIR}/cmake/AutogenBuildConfigHeader.cmake"
)
add_custom_target(
     BuildConfigHeader ALL
     DEPENDS BuildConfigHeaderIsSetup
)

file(GLOB ALL_TARGET_SOURCES src/*.cc)
include_directories(${CGALIncludeFilesList} "${Boost_INCLUDE_DIR}"
                    "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/iB4e"
)
link_libraries("${Boost_LIBRARY_DIR}/libboost_program_options.a" "${Boost_LIBRARY_DIR}/libboost_system.a"
                "${Boost_LIBRARY_DIR}/libboost_log.a"
                "${Boost_LIBRARY_DIR}/libboost_thread.a" "${Boost_LIBRARY_DIR}/libboost_atomic.a"
                "${Boost_LIBRARY_DIR}/libboost_regex.a" "${Boost_LIBRARY_DIR}/libboost_chrono.a")
add_executable(
    pmfe_findmfe_bin ${ALL_TARGET_SOURCES} src/bin-findmfe.cc
)
add_dependencies(pmfe_findmfe_bin BuildConfigHeader)
set_target_properties(
     pmfe_findmfe_bin PROPERTIES
     OUTPUT_NAME "pmfe-findmfe"
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
)

add_executable(
    pmfe_scorer_bin ${ALL_TARGET_SOURCES} src/bin-scorer.cc
)
add_dependencies(pmfe_findmfe_bin BuildConfigHeader)
set_target_properties(
     pmfe_scorer_bin PROPERTIES
     OUTPUT_NAME "pmfe-scorer"
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
)

add_executable(
    pmfe_parametrizer_bin ${ALL_TARGET_SOURCES} src/bin-parametrizer.cc
)
add_dependencies(pmfe_findmfe_bin BuildConfigHeader)
set_target_properties(
     pmfe_parametrizer_bin PROPERTIES
     OUTPUT_NAME "pmfe-parametrizer"
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
)

add_executable(
    pmfe_subopt_bin ${ALL_TARGET_SOURCES} src/bin-subopt.cc
)
add_dependencies(pmfe_findmfe_bin BuildConfigHeader)
set_target_properties(
     pmfe_subopt_bin PROPERTIES
     OUTPUT_NAME "pmfe-subopt"
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
)

add_executable(
    pmfe_tests_bin ${ALL_TARGET_SOURCES} src/bin-tests.cc
)
add_dependencies(pmfe_findmfe_bin BuildConfigHeader)
set_target_properties(
     pmfe_subopt_bin PROPERTIES
     OUTPUT_NAME "pmfe-tests"
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
)

## Install targets (both for binary sub-utilities and the
## Python library to be used with SageMath):
if(IS_DIRECTORY "/opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9")
     set(RHEL_INCLUDE_APPENDS "-I/opt/rh/devtoolset-9/root/usr/include/c++/9")
     set(RHEL_LINKER_APPENDS "-L/opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9")
else()
     set(RHEL_INCLUDE_APPENDS "")
     set(RHEL_LINKER_APPENDS "")
endif()
if(APPLE)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -stdlib=libc++ -fPIC -fopenmp -Wall -g -O3 \
        -DBOOST_LOG_DYN_LINK=0 -DCGAL_HEADER_ONLY -D_GLIBCXX_USE_CXX11_ABI=0 \
        -I/usr/local/include -Iinclude/ -IiB4e/ \
        ${PC_GMP_INCLUDE_DIRS} ${PC_GMPXX_INCLUDE_DIRS} \
        -I${Boost_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS}
        -DPMFE_PACKAGE_VERSION=\"${PMFE_PACKAGE_VERSION}\""
    )
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_CXX_FLAGS} ${CMAKE_EXE_LINKER_FLAGS} \
        -L/usr/local/lib -lgmp -lgmpxx -lCGAL -lm -lboost_filesystem \
        -lboost_program_options -lboost_system -lboost_log -lboost_log_setup \
        -lc++ -lboost_log-mt \
        ${PC_GMP_LIBRARIES} ${PC_GMPXX_LIBRARIES} ${Boost_LIBRARIES}"
    )
elseif(UNIX AND NOT APPLE) # Primary target: Linux (typically RHEL)
    set(CMAKE_CXX_FLAGS
        "-fPIC -fopenmp -Wall -g -O3 -fvisibility=hidden -fvisibility-inlines-hidden \
        -DABI=0 -DBOOST_FILESYSTEM_NO_DEPRECATED -UBOOST_DISABLE_THREADS \
        -DBOOST_LOG_DYN_LINK=1 -BOOST_ALL_DYN_LINK=1 -D_GLIBCXX_USE_CXX11_ABI=1 \
        -DCGAL_HEADER_ONLY -DGAL_USE_LEDA=1 ${RHEL_INCLUDE_APPENDS} ${CMAKE_CXX_FLAGS}"
    )
    set(CMAKE_EXE_LINKER_FLAGS
        "${RHEL_LINKER_APPENDS} -L/usr/lib -L/usr/local/lib \
        ${PC_GMP_LIBRARIES} ${PC_GMPXX_LIBRARIES} ${CMAKE_EXE_LINKER_FLAGS} -L${Boost_LIBRARY_DIR} \
        -static -lboost_filesystem -lboost_program_options -lboost_system \
        -lboost_log -lboost_log_setup -lpthread -lm"
    )
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")

install(
     TARGETS pmfe_findmfe_bin pmfe_scorer_bin pmfe_parametrizer_bin pmfe_subopt_bin pmfe_tests_bin
     DESTINATION ${EXECUTABLE_OUTPUT_PATH}
)

## Backup the old wrapper scripts and copy the new ones in their place:
install(
    SCRIPT cmake/PostBuildAutogenWrapperScript.cmake
)

set(UserRunnerScriptPath "../../PMFECommandRunner.sh")
set(DevRunnerScriptPath  "../../PMFEDeveloperCommandRunner.sh")
set(SageInitScriptPath   "../../init.sage")
set(SageInitScriptNestedPath   "../SageInit/init.sage")

execute_process (
         COMMAND bash -c
                "cp ${UserRunnerScriptPath} ${UserRunnerScriptPath}.bak &&
                cp ${DevRunnerScriptPath} ${DevRunnerScriptPath}.bak &&
                cp ${SageInitScriptPath} ${SageInitScriptPath}.bak &&
                cp ${SageInitScriptNestedPath} ${SageInitScriptNestedPath}.bak"
)
execute_process (
         COMMAND bash -c
                "touch wrapper-runner-script/PMFECommandRunner.sh &&
                cp wrapper-runner-script/PMFECommandRunner.sh ${UserRunnerScriptPath} &&
                touch wrapper-runner-script/PMFEDeveloperCommandRunner.sh &&
                cp wrapper-runner-script/PMFEDeveloperCommandRunner.sh ${DevRunnerScriptPath} &&
                cp wrapper-runner-script/init.sage ${SageInitScriptPath} &&
                cp wrapper-runner-script/init.sage ${SageInitScriptNestedPath}"
)

## Running of unit tests configuration:
function(Func_runUnitTest UnitTestName)
     add_test(NAME "PMFEUnitTestCheck-${UnitTestName}" COMMAND pmfe-tests -n ${UnitTestName} -a -s)
     set_tests_properties(
         "PMFEUnitTestCheck-${UnitTestName}" PROPERTIES
         PASS_REGULAR_EXPRESSION "All tests passed .*"
     )
endfunction(Func_runUnitTest)

if(RUN_UNIT_TESTS)
     enable_testing()
     message("Running *ALL* unit tests -- NOTE: This can take a long time -- ...")
     Func_runUnitTest("16S")
     Func_runUnitTest("5S")
     Func_runUnitTest("asuum")
     Func_runUnitTest("atabira")
     Func_runUnitTest("bbigemina")
     Func_runUnitTest("biological")
     Func_runUnitTest("cdiphtheriae")
     Func_runUnitTest("celegans")
     Func_runUnitTest("combinatorial")
     Func_runUnitTest("dmobilis")
     Func_runUnitTest("ecoli")
     Func_runUnitTest("ecuniculi")
     Func_runUnitTest("ehexamita")
     Func_runUnitTest("garboreum")
     Func_runUnitTest("gardaea")
     Func_runUnitTest("gintestinalis")
     Func_runUnitTest("gmuris")
     Func_runUnitTest("hsapiens")
     Func_runUnitTest("hvolcanii")
     Func_runUnitTest("ldelbrueckii")
     Func_runUnitTest("mfe")
     Func_runUnitTest("onivara")
     Func_runUnitTest("random")
     Func_runUnitTest("rnorvegicus")
     Func_runUnitTest("stokodaii")
     Func_runUnitTest("synthetic")
     Func_runUnitTest("tRNA")
     Func_runUnitTest("vnecatrix")
     Func_runUnitTest("zmays")
endif()
