# Configuring CMake
message("--> Configuring CMake...")

# Configuring CMake > Checking minimum version
message("---> Checking CMake's version...")
message("----> Require 3.16")
message("----> Using ${CMAKE_VERSION}")
cmake_minimum_required(VERSION 3.16)

# Configuring CMake > Selecting compiler
message("---> Checking OS type...")
if (UNIX)
    fatal_error("----> Found UNIX system, please use a Windows system to compile this project !")
else ()
    message("----> Found non-UNIX system, letting CMake autodetect compiler")
endif (UNIX)
message("----> Compiler: ${CMAKE_C_COMPILER}")

# Configuring CMake > Preparing C revision and project itself...
message("---> Setting language to \"C\" and revision to \"C99\" without extensions")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

# Setting up the CMake project itself
project("NibblePoker's Windows Wi-Fi CLI utility" C)

# Adding CTest in order to be able to run tests
message("---> Including CTest...")
include(CTest)

# Getting some info
message("--> System's debugging info printout:")
message("---> System: ${CMAKE_SYSTEM_NAME}")
message("---> Version: ${CMAKE_SYSTEM_VERSION}")
message("---> Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("---> Compiler: ${CMAKE_C_COMPILER}")
message("---> Build type: ${CMAKE_BUILD_TYPE}")
message("---> build types: ${CMAKE_CONFIGURATION_TYPES}")

message("--> Project's debugging info printout:")
message("---> MSCV version: ${MSVC_VERSION}")
message("---> MSCV toolset version: ${MSVC_TOOLSET_VERSION}")
message("---> C Standard version: ${CMAKE_C_STANDARD}")

# Configuring the compilers > Debugging info printout
message("---> Compiler's debugging info printout:")
message("----> CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("----> CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("----> CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
message("----> CMAKE_C_FLAGS_MINSIZEREL: ${CMAKE_C_FLAGS_MINSIZEREL}")
message("----> CMAKE_C_FLAGS_RELWITHDEBINFO: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("----> CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message("----> CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
message("----> CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}")
message("----> CMAKE_STATIC_LINKER_FLAGS: ${CMAKE_STATIC_LINKER_FLAGS}")

message("--> Checking for platform-specific arguments for the compiler")
if (CYGWIN)
    # Does not work, God I hate Cygwin...
    # At least you don't get a multi-MiB file because dead code removal is hard, aww...
    # See: https://cygwin.com/faq/faq.html#faq.programming.static-linking
    #message("---> Detected Cygwin, adding \"-static\" to \"CMAKE_C_FLAGS\"")
    #set(CMAKE_C_FLAGS "-static ${CMAKE_C_FLAGS}")
    message("---> Detected Cygwin.")
    message("---> Good luck, you're on your own there bud...")
elseif (MSVC)
    message("---> Detected MSVC compiler, adding custom flags to compiler and linker...")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Gy /Oy")
    # /Zl seems to affect the implicitly included .lib
    set(CMAKE_C_FLAGS_RELEASE "/Ot ${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os ${CMAKE_C_FLAGS_MINSIZEREL}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ot ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    #string(REPLACE "/O2" "/O3" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    # /MD doesn't affect it !

    # /OPT:NOWIN98 is not documented, but referenced by official doc, but doesn't work.
    # Align apparently cannot be lower than 128 :/
    # Going from 256 to 128 and 16 shaved off 1.5KB roughly
    # Warning: According to the doc, this MAY cause issue on non-driver subsystems.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ALIGN:128 /FILEALIGN:16 /OPT:REF")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /CGTHREADS:8")

    # Adds some extra protection for indirect calls such as the ones used when freeing lists.
    # Adds around .2KB (16->16.2)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /GUARD:CF /DYNAMICBASE")

    # See:
    #  * https://learn.microsoft.com/en-us/cpp/build/reference/subsystem-specify-subsystem?view=msvc-170
    #  * https://learn.microsoft.com/en-us/cpp/build/reference/release-set-the-checksum?view=msvc-170
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /RELEASE /SUBSYSTEM:CONSOLE")

    # The /STUB:<filename> could be fun to play around with
    # https://learn.microsoft.com/en-us/cpp/build/reference/stub-ms-dos-stub-file-name?view=msvc-170

    # The /VERSION
    # https://learn.microsoft.com/en-us/cpp/build/reference/version-version-information?view=msvc-170

    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX")

    # One or both of these options triggers some AV on VirusTotal:
    #  * Rising: HackTool.CoinMiner!8.F154 (TFE:1:avQASvlVVsP)
    #  * SecureAge: Malicious
    #  * Sample: https://www.virustotal.com/gui/file/27b1fc390c1ef5b27351e0c7c752329ee9206329bcbff2bacfae629df60e9c04?nocache=1
    # Welp, after more testing, it also seems to make it unable to use the app on Windows 10 x64, bruh...
    # The gains were half a KB anyway.
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MERGE:.rdata=.data /MERGE:.text=.data")  # /MERGE:.reloc=.data
else ()
    message("---> No actions taken")
endif ()

message("---> Final compiler's debugging info printout:")
message("----> CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("----> CMAKE_C_FLAGS_RELEASE_INIT: ${CMAKE_C_FLAGS_RELEASE_INIT}")
message("----> CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("----> CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
message("----> CMAKE_C_FLAGS_MINSIZEREL: ${CMAKE_C_FLAGS_MINSIZEREL}")
message("----> CMAKE_C_FLAGS_RELWITHDEBINFO: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("----> CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message("----> CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
message("----> CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}")
message("----> CMAKE_STATIC_LINKER_FLAGS: ${CMAKE_STATIC_LINKER_FLAGS}")

# Setting environment variables
message("---> Custom environment variables:")

# > Version
string(TIMESTAMP CURRENT_YEAR_LONG "%Y")
string(TIMESTAMP CURRENT_MONTH "%m")
string(TIMESTAMP CURRENT_DAY "%d")
string(SUBSTRING ${CURRENT_YEAR_LONG} 2 -1 CURRENT_YEAR)
set(ENV{NP_BUILD_WIFI_CLI_VERSION_MAJOR} ${CURRENT_YEAR})
set(ENV{NP_BUILD_WIFI_CLI_VERSION_MINOR} ${CURRENT_MONTH})
set(ENV{NP_BUILD_WIFI_CLI_VERSION_PATCH} ${CURRENT_DAY})
set(ENV{NP_BUILD_WIFI_CLI_VERSION_EXTRA} 0)
add_compile_definitions(NP_BUILD_WIFI_CLI_VERSION_MAJOR=$ENV{NP_BUILD_WIFI_CLI_VERSION_MAJOR})
add_compile_definitions(NP_BUILD_WIFI_CLI_VERSION_MINOR=$ENV{NP_BUILD_WIFI_CLI_VERSION_MINOR})
add_compile_definitions(NP_BUILD_WIFI_CLI_VERSION_PATCH=$ENV{NP_BUILD_WIFI_CLI_VERSION_PATCH})
add_compile_definitions(NP_BUILD_WIFI_CLI_VERSION_EXTRA=$ENV{NP_BUILD_WIFI_CLI_VERSION_EXTRA})
add_compile_definitions(NP_BUILD_WIFI_CLI_VERSION="$ENV{NP_BUILD_WIFI_CLI_VERSION_MAJOR}.$ENV{NP_BUILD_WIFI_CLI_VERSION_MINOR}.$ENV{NP_BUILD_WIFI_CLI_VERSION_PATCH}.$ENV{NP_BUILD_WIFI_CLI_VERSION_EXTRA}")
message("----> NP_BUILD_WIFI_CLI_VERSION_MAJOR: $ENV{NP_BUILD_WIFI_CLI_VERSION_MAJOR}")
message("----> NP_BUILD_WIFI_CLI_VERSION_MINOR: $ENV{NP_BUILD_WIFI_CLI_VERSION_MINOR}")
message("----> NP_BUILD_WIFI_CLI_VERSION_PATCH: $ENV{NP_BUILD_WIFI_CLI_VERSION_PATCH}")
message("----> NP_BUILD_WIFI_CLI_VERSION_EXTRA: $ENV{NP_BUILD_WIFI_CLI_VERSION_EXTRA}")

# > Program's name
set(ENV{NP_BUILD_GENERAL_PROJECT_NAME} ${CMAKE_PROJECT_NAME})
add_compile_definitions(NP_BUILD_GENERAL_PROJECT_NAME="$ENV{NP_BUILD_GENERAL_PROJECT_NAME}")


# TODO: Set MSVC version !

# Preparing targets
file(GLOB_RECURSE src_nibblepoker_c_goodies CONFIGURE_DEPENDS "libs/nibblepoker-c-goodies/src/*.h" "libs/nibblepoker-c-goodies/src/*.c")
file(GLOB_RECURSE src_wifi_commons CONFIGURE_DEPENDS "src/commons/*.h" "src/commons/*.c")

add_executable(wifi_cli src/cli/main.c ${src_nibblepoker_c_goodies} ${src_wifi_commons} rsc/rc/wifi-cli.rc)
set_target_properties(wifi_cli PROPERTIES OUTPUT_NAME "wifi")
target_link_libraries(wifi_cli Wlanapi.lib)

add_executable(wifi_gui src/gui/main.c ${src_nibblepoker_c_goodies} ${src_wifi_commons} rsc/rc/wifi-gui.rc)
set_target_properties(wifi_gui PROPERTIES OUTPUT_NAME "Wi-Fi Toolbox")
target_link_libraries(wifi_gui Wlanapi.lib)


#target_compile_definitions(wifi PUBLIC NP_DEBUG_LOGGING)

# CLion couldn't give two shits about these and actually have them work out of the box, how lovely...
#add_custom_target(run-version COMMAND wifi --build-info COMMENT "Show the version text" DEPENDS wifi)
#add_custom_target(run-build COMMAND wifi --version COMMENT "Show the build info" DEPENDS wifi)
#add_custom_target(run-help-root COMMAND wifi --help COMMENT "Show the root's help text" DEPENDS wifi)
