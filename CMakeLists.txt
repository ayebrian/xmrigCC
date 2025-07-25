cmake_minimum_required(VERSION 3.10)
project(xmrigcc)

set(CMAKE_BUILD_TYPE "Release")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "-- XMRigCC: Found ccache package... Activating...")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

option(WITH_HWLOC           "Enable hwloc support" ON)
option(WITH_CN_LITE         "Enable CryptoNight-Lite algorithms family" ON)
option(WITH_CN_HEAVY        "Enable CryptoNight-Heavy algorithms family" ON)
option(WITH_CN_PICO         "Enable CryptoNight-Pico algorithm" ON)
option(WITH_CN_FEMTO        "Enable CryptoNight-UPX2 algorithm" ON)
option(WITH_CN_GPU          "Enable CryptoNight-GPU algorithm" ON)
option(WITH_RANDOMX         "Enable RandomX algorithms family" ON)
option(WITH_ARGON2          "Enable Argon2 algorithms family" ON)
option(WITH_KAWPOW          "Enable KawPow algorithms family" ON)
option(WITH_GHOSTRIDER      "Enable GhostRider algorithm" ON)
option(WITH_HTTP            "Enable HTTP protocol support (client/server)" ON)
option(WITH_DEBUG_LOG       "Enable debug log output" OFF)
option(WITH_TLS             "Enable OpenSSL support" ON)
option(WITH_ASM             "Enable ASM PoW implementations" ON)
option(WITH_MSR             "Enable MSR mod & 1st-gen Ryzen fix" ON)
option(WITH_ENV_VARS        "Enable environment variables support in config file" ON)
option(WITH_EMBEDDED_CONFIG "Enable internal embedded JSON config" OFF)
option(WITH_OPENCL          "Enable OpenCL backend" ON)
set(WITH_OPENCL_VERSION 200 CACHE STRING "Target OpenCL version")
set_property(CACHE WITH_OPENCL_VERSION PROPERTY STRINGS 120 200 210 220)
option(WITH_CUDA            "Enable CUDA backend" ON)
option(WITH_NVML            "Enable NVML (NVIDIA Management Library) support (only if CUDA backend enabled)" ON)
option(WITH_ADL             "Enable ADL (AMD Display Library) or sysfs support (only if OpenCL backend enabled)" ON)
option(WITH_STRICT_CACHE    "Enable strict checks for OpenCL cache" ON)
option(WITH_INTERLEAVE_DEBUG_LOG "Enable debug log for threads interleave" OFF)
option(WITH_PROFILING       "Enable profiling for developers" OFF)
option(WITH_SSE4_1          "Enable SSE 4.1 for Blake2" ON)
option(WITH_AVX2            "Enable AVX2 for Blake2" ON)
option(WITH_VAES            "Enable VAES instructions for Cryptonight" ON)
option(WITH_SECURE_JIT      "Enable secure access to JIT memory" OFF)
option(WITH_DMI             "Enable DMI/SMBIOS reader" ON)

option(WITH_ZLIB            "Enabled gzip compression on CC (client/server)" OFF)
option(WITH_CC_CLIENT       "CC Client" ON)
option(WITH_CC_SHELL_EXEC   "CC Client allow shell execute" ON)
option(WITH_CC_SERVER       "CC Server" ON)
option(WITH_HTTPLIB_POLL    "Use poll.h (recommended) instead of old plain sockets for HTTP" ON)

option(BUILD_STATIC         "Build static binary" OFF)
option(ARM_V8               "Force ARMv8 (64 bit) architecture, use with caution if automatic detection fails, but you sure it may work" OFF)
option(ARM_V7               "Force ARMv7 (32 bit) architecture, use with caution if automatic detection fails, but you sure it may work" OFF)
option(HWLOC_DEBUG          "Enable hwloc debug helpers and log" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if(NOT MINER_EXECUTABLE_NAME)
    set(MINER_EXECUTABLE_NAME "xmrigMiner" CACHE STRING "Miner executable file name")
endif(NOT MINER_EXECUTABLE_NAME)

if(NOT DAEMON_EXECUTABLE_NAME)
    set(DAEMON_EXECUTABLE_NAME "xmrigDaemon" CACHE STRING "Daemon executable file name")
endif(NOT DAEMON_EXECUTABLE_NAME)

include (CheckIncludeFile)
include (cmake/cpu.cmake)
include (cmake/os.cmake)
include (src/base/base.cmake)
include (src/backend/backend.cmake)


set(HEADERS
    "${HEADERS_BASE}"
    "${HEADERS_BASE_HTTP}"
    "${HEADERS_BACKEND}"
    src/App.h
    src/core/config/Config_default.h
    src/core/config/Config_platform.h
    src/core/config/Config.h
    src/core/config/ConfigTransform.h
    src/core/config/usage.h
    src/core/Controller.h
    src/core/Miner.h
    src/core/Taskbar.h
    src/net/interfaces/IJobResultListener.h
    src/net/JobResult.h
    src/net/JobResults.h
    src/net/Network.h
    src/net/strategies/DonateStrategy.h
    src/Summary.h
    src/version.h
   )

set(HEADERS_CRYPTO
    src/backend/common/interfaces/IMemoryPool.h
    src/crypto/cn/asm/CryptonightR_template.h
    src/crypto/cn/c_blake256.h
    src/crypto/cn/c_groestl.h
    src/crypto/cn/c_jh.h
    src/crypto/cn/c_skein.h
    src/crypto/cn/CnAlgo.h
    src/crypto/cn/CnCtx.h
    src/crypto/cn/CnHash.h
    src/crypto/cn/CryptoNight_monero.h
    src/crypto/cn/CryptoNight_test.h
    src/crypto/cn/CryptoNight.h
    src/crypto/cn/groestl_tables.h
    src/crypto/cn/hash.h
    src/crypto/cn/skein_port.h
    src/crypto/cn/soft_aes.h
    src/crypto/common/HugePagesInfo.h
    src/crypto/common/MemoryPool.h
    src/crypto/common/Nonce.h
    src/crypto/common/portable/mm_malloc.h
    src/crypto/common/VirtualMemory.h
   )

if (XMRIG_ARM)
    set(HEADERS_CRYPTO "${HEADERS_CRYPTO}" src/crypto/cn/CryptoNight_arm.h)
else()
    set(HEADERS_CRYPTO "${HEADERS_CRYPTO}" src/crypto/cn/CryptoNight_x86.h)
endif()

set(SOURCES
    "${SOURCES_BASE}"
    "${SOURCES_BASE_HTTP}"
    "${SOURCES_BACKEND}"
    src/App.cpp
    src/core/config/Config.cpp
    src/core/config/ConfigTransform.cpp
    src/core/Controller.cpp
    src/core/Miner.cpp
    src/core/Taskbar.cpp
    src/net/JobResults.cpp
    src/net/Network.cpp
    src/net/strategies/DonateStrategy.cpp
    src/Summary.cpp
    src/xmrig.cpp
   )

set(SOURCES_CRYPTO
    src/crypto/cn/c_blake256.c
    src/crypto/cn/c_groestl.c
    src/crypto/cn/c_jh.c
    src/crypto/cn/c_skein.c
    src/crypto/cn/CnCtx.cpp
    src/crypto/cn/CnHash.cpp
    src/crypto/common/HugePagesInfo.cpp
    src/crypto/common/MemoryPool.cpp
    src/crypto/common/Nonce.cpp
    src/crypto/common/VirtualMemory.cpp
   )

if (CMAKE_C_COMPILER_ID MATCHES GNU)
    set_source_files_properties(src/crypto/cn/CnHash.cpp PROPERTIES COMPILE_FLAGS "-Ofast -fno-tree-vectorize")
endif()

if (WITH_VAES)
    add_definitions(-DXMRIG_VAES)
    set(HEADERS_CRYPTO "${HEADERS_CRYPTO}" src/crypto/cn/CryptoNight_x86_vaes.h)
    set(SOURCES_CRYPTO "${SOURCES_CRYPTO}" src/crypto/cn/CryptoNight_x86_vaes.cpp)
    if (CMAKE_C_COMPILER_ID MATCHES GNU OR CMAKE_C_COMPILER_ID MATCHES Clang)
        set_source_files_properties(src/crypto/cn/CryptoNight_x86_vaes.cpp PROPERTIES COMPILE_FLAGS "-Ofast -fno-tree-vectorize -mavx2 -mvaes")
    endif()
endif()

if (WITH_HWLOC)
    list(APPEND HEADERS_CRYPTO
        src/crypto/common/NUMAMemoryPool.h
        )

    list(APPEND SOURCES_CRYPTO
        src/crypto/common/NUMAMemoryPool.cpp
        src/crypto/common/VirtualMemory_hwloc.cpp
        )
endif()

if (XMRIG_OS_WIN)
    list(APPEND SOURCES_OS
        res/app.rc
        src/App_win.cpp
        src/crypto/common/VirtualMemory_win.cpp
        )

    set(EXTRA_LIBS ws2_32 psapi iphlpapi userenv dbghelp)
elseif (XMRIG_OS_APPLE)
    list(APPEND SOURCES_OS
        src/App_unix.cpp
        src/crypto/common/VirtualMemory_unix.cpp
        )

    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORESERVICES_LIBRARY CoreServices)
    set(EXTRA_LIBS ${IOKIT_LIBRARY} ${CORESERVICES_LIBRARY})
else()
    list(APPEND SOURCES_OS
        src/App_unix.cpp
        src/crypto/common/VirtualMemory_unix.cpp
        )

    if (XMRIG_OS_ANDROID)
        set(EXTRA_LIBS dl log)
    elseif (XMRIG_OS_LINUX)
        list(APPEND SOURCES_OS
            src/crypto/common/LinuxMemory.h
            src/crypto/common/LinuxMemory.cpp
            )

        set(EXTRA_LIBS pthread rt dl)
    elseif (XMRIG_OS_FREEBSD)
        set(EXTRA_LIBS kvm pthread)
    endif()
endif()

add_definitions(-DMINER_EXECUTABLE_NAME=${MINER_EXECUTABLE_NAME})
add_definitions(-DXMRIG_MINER_PROJECT -DXMRIG_JSON_SINGLE_LINE_ARRAY)
add_definitions(-D__STDC_FORMAT_MACROS -DUNICODE -D_FILE_OFFSET_BITS=64)

find_package(UV REQUIRED)

include(cmake/flags.cmake)
include(cmake/cn-gpu.cmake)
include(cmake/randomx.cmake)
include(cmake/argon2.cmake)
include(cmake/kawpow.cmake)
include(cmake/ghostrider.cmake)
include(cmake/OpenSSL.cmake)
include(cmake/asm.cmake)
include(cmake/cc.cmake)

if (WITH_CN_LITE)
    add_definitions(/DXMRIG_ALGO_CN_LITE)
endif()

if (WITH_CN_HEAVY)
    add_definitions(/DXMRIG_ALGO_CN_HEAVY)
endif()

if (WITH_CN_PICO)
    add_definitions(/DXMRIG_ALGO_CN_PICO)
endif()

if (WITH_CN_FEMTO)
    add_definitions(/DXMRIG_ALGO_CN_FEMTO)
endif()

if (WITH_EMBEDDED_CONFIG)
    add_definitions(/DXMRIG_FEATURE_EMBEDDED_CONFIG)
endif()

include(src/hw/api/api.cmake)
include(src/hw/dmi/dmi.cmake)

include_directories(src)
include_directories(src/3rdparty)
include_directories(${UV_INCLUDE_DIR})

if (WITH_DEBUG_LOG)
    add_definitions(/DAPP_DEBUG)
endif()

add_executable(xmrigMiner ${HEADERS} ${SOURCES} ${SOURCES_OS} ${HEADERS_CRYPTO} ${SOURCES_CRYPTO} ${SOURCES_SYSLOG} ${TLS_SOURCES} ${XMRIG_ASM_SOURCES} ${SOURCES_CC_CLIENT} ${SOURCES_CC_COMMON})
target_link_libraries(xmrigMiner ${XMRIG_ASM_LIBRARY} ${OPENSSL_LIBRARIES} ${UV_LIBRARIES} ${EXTRA_LIBS} ${CPUID_LIB} ${ARGON2_LIBRARY} ${ETHASH_LIBRARY} ${GHOSTRIDER_LIBRARY} ${ZLIB_LIBRARIES})
set_target_properties(xmrigMiner PROPERTIES OUTPUT_NAME ${MINER_EXECUTABLE_NAME})

if (CMAKE_CXX_COMPILER_ID MATCHES Clang AND CMAKE_BUILD_TYPE STREQUAL Release AND NOT CMAKE_GENERATOR STREQUAL Xcode)
    add_custom_command(TARGET xmrigMiner POST_BUILD COMMAND ${CMAKE_STRIP} ${MINER_EXECUTABLE_NAME})
endif()

if (WITH_CC_CLIENT)
    add_executable(xmrigDaemon src/cc/XMRigd.cpp res/app.rc)
    set_target_properties(xmrigDaemon PROPERTIES OUTPUT_NAME ${DAEMON_EXECUTABLE_NAME})
endif()

if (WITH_CC_SERVER)
    add_executable(xmrigServer ${SOURCES_CC_SERVER} ${SOURCES_CC_COMMON} ${SOURCES_SYSLOG} res/app.rc)
    target_link_libraries(xmrigServer ${XMRIG_ASM_LIBRARY} ${OPENSSL_LIBRARIES} ${UV_LIBRARIES} ${EXTRA_LIBS} ${CPUID_LIB} ${ARGON2_LIBRARY} ${ZLIB_LIBRARIES})

    set(CLIENT_VERSIONS
            client-updates/linux-static-amd64
            client-updates/linux-static-arm64
            client-updates/linux-static-i386
            client-updates/linux-dynamic-amd64
            client-updates/freebsd-static-amd64
            client-updates/android-dynamic-arm64
            client-updates/macos-x64_64
            client-updates/gcc-win64
            client-updates/gcc-win32
            client-updates/mvc-win64)
    add_custom_command(TARGET xmrigServer POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CLIENT_VERSIONS})
endif()
