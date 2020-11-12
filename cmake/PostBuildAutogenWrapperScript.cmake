#### PostBuildAutogenWrapperScript.cmake : Create the post-build wrapper / runner script source
####                                       to run the PMFE utilities with the correct
####                                       absolute paths local to this build.
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
## Syntax: '--SUBST_PLACEHOLDER_NAME---ActualValue' denotes that we will replace the string
##         'SUBST_PLACEHOLDER_NAME' with 'ActualValue' below.
list(APPEND PMFEWrapperRunnerScriptSubstsList "--PMFE_BUILD_TIMESTAMP---${BuildTimeStamp}")
list(APPEND PMFEWrapperRunnerScriptSubstsList "--PMFE_BUILD_GIT_COMMIT_HASH---${BuildGitCommitHash}")
list(APPEND PMFEWrapperRunnerScriptSubstsList "--PMFE_BUILD_GIT_COMMIT_DATE---${BuildGitCommitDate}")
list(APPEND PMFEWrapperRunnerScriptSubstsList "--PMFE_BUILD_ABS_BINARY_DIR_PATH---${BuildAbsPath}")

## Copy the stub header file to the build-time location:

set(PMFEWrapperRunnerScriptStubPath "wrapper-runner-script/PMFECommandRunner.sh.in")
set(PMFEWrapperRunnerScriptNewPath  "wrapper-runner-script/PMFECommandRunner.sh")
execute_process (
     COMMAND bash -c "cp ${PMFEWrapperRunnerScriptStubPath} ${PMFEWrapperRunnerScriptNewPath}"
)

set(PMFEWrapperDevRunnerScriptStubPath "wrapper-runner-script/PMFEDeveloperCommandRunner.sh.in")
set(PMFEWrapperDevRunnerScriptNewPath  "wrapper-runner-script/PMFEDeveloperCommandRunner.sh")
execute_process (
     COMMAND bash -c "cp ${PMFEWrapperDevRunnerScriptStubPath} ${PMFEWrapperDevRunnerScriptNewPath}"
)

## Substitute the placeholders in that file with the live build config information:
function(FUNC_SUBST_PMFE_WRAPPER_RUNNER_SCRIPT_PARAM SUBST_VAR_NAME SUBST_VAR_VALUE OUTFILE_PATH)
     message(STATUS "Substituting ${SUBST_VAR_NAME} ==> ${SUBST_VAR_VALUE} in ${OUTFILE_PATH} ...")
     if(APPLE)
         execute_process (
             COMMAND bash -c "gsed -i -- \"s|${SUBST_VAR_NAME}|${SUBST_VAR_VALUE}|g\" ${OUTFILE_PATH}"
         )
     elseif(UNIX AND NOT APPLE)
         execute_process (
             COMMAND bash -c "sed -i -- \"s|${SUBST_VAR_NAME}|${SUBST_VAR_VALUE}|g\" ${OUTFILE_PATH}"
         )
     endif()
endfunction(FUNC_SUBST_PMFE_WRAPPER_RUNNER_SCRIPT_PARAM)

function(FUNC_SUBST_PMFE_WRAPPER_RUNNER_SCRIPT_PARAM_FROM_SPEC FULL_CONFIG_SUBST_SPEC OUTFILE_PATH)
     string(REGEX MATCH "--[^\ ]+---" FuncInput_localParamName "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX REPLACE "-" "" FuncInput_localParamName "${FuncInput_localParamName}")
     string(REGEX MATCH "---[^\ ]+" FuncInput_localParamValue "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX REPLACE "---" "" FuncInput_localParamValue "${FuncInput_localParamValue}")
     string(STRIP "${FuncInput_localParamValue}" FuncInput_localParamValue)
     func_subst_pmfe_wrapper_runner_script_param(
          "${FuncInput_localParamName}"
          "${FuncInput_localParamValue}"
          "${OUTFILE_PATH}"
     )
endfunction(FUNC_SUBST_PMFE_WRAPPER_RUNNER_SCRIPT_PARAM_FROM_SPEC)

foreach(PMFE_RUNNER_SCRIPT_SUBST_SPEC ${PMFEWrapperRunnerScriptSubstsList})
     func_subst_pmfe_wrapper_runner_script_param_from_spec("${PMFE_RUNNER_SCRIPT_SUBST_SPEC}" "${PMFEWrapperRunnerScriptNewPath}")
     func_subst_pmfe_wrapper_runner_script_param_from_spec("${PMFE_RUNNER_SCRIPT_SUBST_SPEC}" "${PMFEWrapperDevRunnerScriptNewPath}")
endforeach()

## Backup the old wrapper scripts and copy the new ones in their place:
set(UserRunnerScriptPath       "../../PMFECommandRunner.sh")
set(DevRunnerScriptPath        "../../PMFEDeveloperCommandRunner.sh")
set(SageInitScriptPath         "../../init.sage")
set(SageInitScriptNestedPath   "../SageInit/init.sage")

execute_process(COMMAND bash -c "cp ${UserRunnerScriptPath} ${UserRunnerScriptPath}.bak")
execute_process(COMMAND bash -c "cp ${DevRunnerScriptPath} ${DevRunnerScriptPath}.bak")
execute_process(COMMAND bash -c "cp ${SageInitScriptPath} ${SageInitScriptPath}.bak")
execute_process(COMMAND bash -c "cp ${SageInitScriptNestedPath} ${SageInitScriptNestedPath}.bak")
execute_process(COMMAND bash -c "cp wrapper-runner-script/PMFECommandRunner.sh ${UserRunnerScriptPath}")
execute_process(COMMAND bash -c "cp wrapper-runner-script/PMFEDeveloperCommandRunner.sh ${DevRunnerScriptPath}")
execute_process(COMMAND bash -c "cp wrapper-runner-script/init.sage ${SageInitScriptPath}")
execute_process(COMMAND bash -c "cp wrapper-runner-script/init.sage ${SageInitScriptNestedPath}")

message(STATUS "Installed the autogenerated wrapper (binary runner) scripts into: ")
message(STATUS "  >> ${PMFEWrapperRunnerScriptNewPath} [user runner script]")
message(STATUS "  >> ${UserRunnerScriptPath} [user runner script]")
message(STATUS "  >> ${PMFEWrapperDevRunnerScriptNewPath} [developer script]")
message(STATUS "  >> ${DevRunnerScriptPath} [developer script]")
