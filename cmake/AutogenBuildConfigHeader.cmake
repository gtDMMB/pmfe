#### AutogenBuildConfigHeader.cmake : Create the PMFE build config header.
#### Author: Maxie D. Schmidt (github.com/maxieds)
#### Created: 2020.11.02

## Set the build time datestamp for reference:
execute_process (
    COMMAND bash -c "date +'%F @ %T%p (%Y.%m.%d-%H%M%S)'"
    OUTPUT_VARIABLE BuildTimeStamp
)

if(APPLE)
     execute_process(
          COMMAND bash -c "greadlink -f ."
          OUTPUT_VARIABLE BuildAbsPath
     )
elseif(UNIX AND NOT APPLE)
     execute_process(
          COMMAND bash -c "readlink -f ."
          OUTPUT_VARIABLE BuildAbsPath
     )
endif()

execute_process(
     COMMAND bash -c "echo `uname -o`::`uname -i`"
     OUTPUT_VARIABLE SystemVersion
)

## Create a record of which GitHub commit or tag the PMFE sources reference:
execute_process (
    COMMAND bash -c "git --no-pager log -1 --format='%H [-- %h --]'"
    OUTPUT_VARIABLE BuildGitCommitHash
)
execute_process (
    COMMAND bash -c "git --no-pager log -1 --format='%ai'"
    OUTPUT_VARIABLE BuildGitCommitDate
)

## Define all of the file placeholders we will substitute the live configuration data into:
## Syntax: "--SUBST_PLACEHOLDER_NAME---ActualValue" denotes that we will replace the string
##         "SUBST_PLACEHOLDER_NAME" with "ActualValue" below.
list(APPEND PMFEConfigHeaderSubstsList "--__CMAKE_VERSION---CMake::v${CMAKE_VERSION}")
list(APPEND PMFEConfigHeaderSubstsList "--__SYSTEM_VERSION---${SystemVersion}")
list(APPEND PMFEConfigHeaderSubstsList "--__PMFE_BUILD_GIT_COMMIT_HASH---${BuildGitCommitHash}")
list(APPEND PMFEConfigHeaderSubstsList "--__PMFE_BUILD_GIT_COMMIT_DATE---${BuildGitCommitDate}")
list(APPEND PMFEConfigHeaderSubstsList "--__PMFE_BUILD_DATESTAMP---${BuildTimeStamp}")
list(APPEND PMFEConfigHeaderSubstsList "--__PMFE_BUILD_ABS_WORKING_DIR---${BuildAbsPath}")

## Copy the stub header file to the build-time location:
set(PMFEBuildConfigHeaderStubPath "include/pmfe_build_config.h.in")
set(PMFEBuildConfigNewHeaderPath "include/pmfe_build_config.h")
execute_process (
         COMMAND bash -c "cp ${PMFEBuildConfigHeaderStubPath} ${PMFEBuildConfigNewHeaderPath}"
)

## Substitute the placeholders in that file with the live build config information:
function(FUNC_SUBST_PMFE_CONFIG_HEADER_PARAM SUBST_VAR_NAME SUBST_VAR_VALUE)
     message(STATUS "Substituting ${SUBST_VAR_NAME} ==> ${SUBST_VAR_VALUE} in ${PMFEBuildConfigNewHeaderPath} ...")
     execute_process (
         COMMAND bash -c "sed -i -- \"s|${SUBST_VAR_NAME}|${SUBST_VAR_VALUE}|g\" ${PMFEBuildConfigNewHeaderPath}"
     )
endfunction(FUNC_SUBST_PMFE_CONFIG_HEADER_PARAM)

function(FUNC_SUBST_PMFE_CONFIG_HEADER_PARAM_FROM_SPEC FULL_CONFIG_SUBST_SPEC)
     string(REGEX MATCH "--[^\ ]+---" FuncInput_localParamName "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX REPLACE "-" "" FuncInput_localParamName "${FuncInput_localParamName}")
     string(REGEX MATCH "---[^\ ]+" FuncInput_localParamValue "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX REPLACE "---" "" FuncInput_localParamValue "${FuncInput_localParamValue}")
     string(REGEX REPLACE "\\." "\\\\." FuncInput_localParamValue "${FuncInput_localParamValue}")
     string(STRIP "${FuncInput_localParamValue}" FuncInput_localParamValue)
     func_subst_pmfe_config_header_param("${FuncInput_localParamName}" "${FuncInput_localParamValue}")
endfunction(FUNC_SUBST_PMFE_CONFIG_HEADER_PARAM_FROM_SPEC)

foreach(PMFE_CONFIG_HEADER_SUBST_SPEC ${PMFEConfigHeaderSubstsList})
     func_subst_pmfe_config_header_param_from_spec("${PMFE_CONFIG_HEADER_SUBST_SPEC}")
endforeach()
