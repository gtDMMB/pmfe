#### AutogenBuildConfigHeader.cmake : Create the PMFE build config header.
#### Author: Maxie D. Schmidt (github.com/maxieds)
#### Created: 2020.11.02

## Set the absolute location of the path to the PMFE sources:
if(APPLE)
    execute_process (
        COMMAND bash -c "greadlink -f ."
        OUTPUT_VARIABLE BuildAbsPath
    )
elseif(UNIX AND NOT APPLE)
    execute_process (
        COMMAND bash -c "readlink -f ."
        OUTPUT_VARIABLE BuildAbsPath
    )
endif()

## Set the build time datestamp for reference:
execute_process (
    COMMAND bash -c "date +'%F @ %T%p (%Y.%m.%d-%H%M%S)'"
    OUTPUT_VARIABLE BuildTimeStamp
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
## Syntax: "[SUBST_PLACEHOLDER_NAME]->ActualValue" denotes that we will replace the string
##         '${SUBST_PLACEHOLDER_NAME}' with 'ActualValue' below.
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_VERSION]->${CMAKE_VERSION}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_SYSTEM_VERSION]->${CMAKE_SYSTEM_VERSION}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_SYSTEM_PROCESSOR]->${CMAKE_SYSTEM_PROCESSOR}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_HOST_SYSTEM_NAME]->${CMAKE_HOST_SYSTEM_NAME}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_C_FLAGS]->${CMAKE_C_FLAGS}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_CXX_FLAGS]->${CMAKE_CXX_FLAGS}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_BUILD_TYPE]->${CMAKE_BUILD_TYPE}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_EXE_LINKER_FLAGS]->${CMAKE_EXE_LINKER_FLAGS}")
list(APPEND PMFEConfigHeaderSubstsList "[CMAKE_SHARED_LINKER_FLAGS]->${CMAKE_SHARED_LINKER_FLAGS}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_PACKAGE_NAME]->${PMFE_PACKAGE_NAME}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_PACKAGE_VERSION]->${PMFE_PACKAGE_VERSION}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_PACKAGE_STRING]->${PMFE_PACKAGE_STRING}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_BUILD_GIT_COMMIT_HASH]->${BuildGitCommitHash}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_BUILD_GIT_COMMIT_DATE]->${BuildGitCommitDate}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_BUILD_DATESTAMP]->${BuildTimeStamp}")
list(APPEND PMFEConfigHeaderSubstsList "[PMFE_BUILD_ABS_WORKING_DIR]->${BuildAbsPath}")

## Copy the stub header file to the build-time location:
set(PMFEBuildConfigHeaderStubPath "${buildAbsPath}/include/pmfe_build_config.h.in")
set(PMFEBuildConfigNewHeaderPath "${buildAbsPath}/include/pmfe_build_config.h")
file(
    COPY ${PMFEBuildConfigHeaderStubPath}
    DESTINATION ${PMFEBuildConfigNewHeaderPath}
)

## Substitute the placeholders in that file with the live build config information:
function(Func_substPMFEConfigHeaderParam "${SUBST_VAR_NAME}" "${SUBST_VAR_VALUE}")
     message(DEBUG "Substituting ${SUBST_VAR_NAME} -> ${SUBST_VAR_VALUE} in ${PMFEBuildConfigNewHeaderPath} ...")
     execute_process (
         COMMAND bash -c "sed -i 's/\$\{${SUBST_VAR_NAME}\}/${SUBST_VAR_VALUE}/' ${PMFEBuildConfigNewHeaderPath}"
     )
endfunction(Func_substPMFEConfigHeaderParam)

function(Func_substPMFEConfigHeaderParamFromSpec "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX "(\[[^\ ]+\])->" funcInput_localParamName "${FULL_CONFIG_SUBST_SPEC}")
     string(REGEX "->[^\ ]+$" funcInput_localParamValue "${FULL_CONFIG_SUBST_SPEC}")
     Func_substPMFEConfigHeaderParam("${funcInput_localParamName}" "${funcInput_localParamValue}")
endfunction(Func_substPMFEConfigHeaderParamFromSpec)

foreach(PMFE_CONFIG_HEADER_SUBST_SPEC ${PMFEConfigHeaderSubstsList})
     Func_substPMFEConfigHeaderParamFromSpec("${PMFE_CONFIG_HEADER_SUBST_SPEC}")
endforeach()
