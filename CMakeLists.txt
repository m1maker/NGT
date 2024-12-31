# CMakeList.txt : CMake project for NGT
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
cmake_policy(SET CMP0002 NEW)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NGT")
add_compile_definitions("NGT" UNICODE)
add_compile_definitions("NGTW" UNICODE)
add_compile_definitions("NGTW" NGTW)
set(CMAKE_INSTALL_PREFIX Release)


string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

add_definitions(-DPOCO_STATIC)
add_definitions(-DENABLE_CRYPTO=OFF)
add_definitions(-DENABLE_JWT=OFF)
add_definitions(-DENABLE_APACHECONNECTOR=OFF)
add_definitions(-DENABLE_DATA_MYSQL=OFF)
add_definitions(-DENABLE_DATA=OFF)
add_definitions(-DENABLE_FOUNDATION=ON)
add_definitions(-DENABLE_ENCODINGS=OFF)
add_definitions(-DENABLE_ENCODINGS_COMPILER=OFF)
add_definitions(-DENABLE_XML=OFF)
add_definitions(-DENABLE_JSON=OFF)
add_definitions(-DENABLE_MONGODB=OFF)
add_definitions(-DENABLE_PROMETHEUS=OFF)

if(WIN32)
  set(POCO_NETSSL Poco::NetSSLWin)
else()
  set(POCO_NETSSL Poco::NetSSL)
endif()

# OpenSSL configuration
set(OPENSSL_USE_STATIC_LIBS ON)
set(OPENSSL_MSVC_STATIC_RT ON)

if(WIN32)
  # Check for vcpkg at C:\vcpkg
  if(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg at C:/vcpkg")
  else()
    # Install vcpkg automatically
    execute_process(
      COMMAND powershell -Command "cd C:\\; git clone https://github.com/Microsoft/vcpkg.git; .\\vcpkg\\bootstrap-vcpkg.bat"
      RESULT_VARIABLE vcpkg_install_result
      OUTPUT_VARIABLE vcpkg_install_output
      ERROR_VARIABLE vcpkg_install_error
    )
    if(NOT vcpkg_install_result EQUAL 0)
      message(FATAL_ERROR "Failed to install vcpkg: ${vcpkg_install_error}")
    endif()

    # Install OpenSSL
    execute_process(
      COMMAND powershell -Command "C:\\vcpkg\\vcpkg install openssl:x64-windows-static"
      RESULT_VARIABLE openssl_install_result
      OUTPUT_VARIABLE openssl_install_output
      ERROR_VARIABLE openssl_install_error
    )
    if(NOT openssl_install_result EQUAL 0)
      message(FATAL_ERROR "Failed to install OpenSSL: ${openssl_install_error}")
    endif()

    # Set the toolchain file
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg at C:/vcpkg")
  endif()

    set(OPENSSL_ROOT_DIR "${CMAKE_TOOLCHAIN_FILE}/../../../installed/x64-windows-static")
else()
    if(APPLE)
        # For macOS, use Homebrew's OpenSSL
        execute_process(
            COMMAND brew --prefix openssl
            OUTPUT_VARIABLE OPENSSL_ROOT_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT OPENSSL_ROOT_DIR)
            message(FATAL_ERROR "OpenSSL not found. Please install it using 'brew install openssl'")
        endif()
    endif()
    # For Linux, the system's package manager should be used (no special handling required)
endif()

find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL and try again.")
endif()

message(STATUS "OpenSSL_FOUND: ${OpenSSL_FOUND}")
message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")


include(FetchContent)
FetchContent_Declare(
  angelscript
  GIT_REPOSITORY "https://github.com/codecat/angelscript-mirror"
  GIT_TAG "f2034769ce887367e97a5fbaced5b14aa8039fd3"
  SOURCE_SUBDIR sdk/angelscript/projects/cmake SYSTEM
)
FetchContent_MakeAvailable(angelscript)


set(SDL_STATIC_DEFAULT ON)
set(SDL_SHARED_DEFAULT OFF)



include(ExternalProject)

ExternalProject_Add(SDL3
    PREFIX "${CMAKE_BINARY_DIR}/external"
    GIT_REPOSITORY    "https://github.com/libsdl-org/SDL"
    GIT_TAG           "6771a6020da5052e22930c36cb1070a4c048ba3c"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/installed -DSDL_STATIC=ON -DMSVC_RUNTIME_LIBRARY=MultiThreaded -DFORCE_STATIC_VCRT=ON
    )


ExternalProject_Add(
    phonon
    URL https://github.com/ValveSoftware/steam-audio/releases/download/v4.5.3/steamaudio_4.5.3.zip
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/phonon-src
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)




FetchContent_Declare(SRAL
  GIT_REPOSITORY "https://github.com/m1maker/SRAL"
  GIT_TAG origin/main SYSTEM)
FetchContent_MakeAvailable(SRAL)


set(ENABLE_NETSSL ON CACHE BOOL "Enable NetSSL")
set(ENABLE_NETSSL_WIN ON CACHE BOOL "Enable NetSSL_WIN")
set(ENABLE_CRYPTO ON CACHE BOOL "Enable Crypto")
set(POCO_MT ON)
set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(Poco
  GIT_REPOSITORY "https://github.com/pocoproject/poco"
  GIT_TAG "cd06553d89aee4bf7b5c820199121cd25d5bce94"
  CMAKE_ARGS
    -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
    -DOPENSSL_USE_STATIC_LIBS=${OPENSSL_USE_STATIC_LIBS}
    -DENABLE_NETSSL=${ENABLE_NETSSL}
    -DENABLE_NETSSL_WIN=${ENABLE_NETSSL_WIN}
    -DENABLE_CRYPTO=${ENABLE_CRYPTO}
    -DPOCO_STATIC=${POCO_STATIC}
    -DENABLE_DATA_MYSQL=${ENABLE_DATA_MYSQL}
    -DENABLE_DATA_POSTGRESQL=${ENABLE_DATA_POSTGRESQL}
    -DENABLE_APACHECONNECTOR=${ENABLE_APACHECONNECTOR}
    -DENABLE_CPPPARSER=${ENABLE_CPPPARSER}
    -DENABLE_REDIS=${ENABLE_REDIS}
)
FetchContent_MakeAvailable(Poco)
set(POCO_STATIC ON)


FetchContent_Declare(enet
  GIT_REPOSITORY "https://github.com/lsalzman/enet"
  GIT_TAG origin/master SYSTEM)
FetchContent_MakeAvailable(enet)


include_directories(
  ${angelscript_SOURCE_DIR}/sdk/angelscript/include
  ${Poco_SOURCE_DIR}/Net/include
  ${poco_SOURCE_DIR}/Crypto/include
  ${Poco_SOURCE_DIR}/Foundation/include
  ${enet_SOURCE_DIR}/include
  ${SRAL_SOURCE_DIR}/Include
  ${CMAKE_BINARY_DIR}/external/installed/include
  ${CMAKE_BINARY_DIR}/phonon-src/include
)
if(WIN32)
  include_directories(
    ${Poco_SOURCE_DIR}/NetSSL_Win/include
  )
else()
  include_directories(
    ${OPENSSL_INCLUDE_DIR}
    ${Poco_SOURCE_DIR}/NetSSL_OpenSSL/include
  )
endif()

set(NGT_SOURCES
  "SRC/ngt.cpp" 
  "SRC/main.cpp"
  "SRC/ngtreg.cpp"
  "SRC/sound.cpp"
  "SRC/miniaudio.h"
  "SRC/cmp.c"
  "SRC/sqlite3.c"
  "SRC/stb_vorbis.c"
  "SRC/as_class.cpp"
  "SRC/dlibrary.cpp"
  "SRC/docgen.cpp"
  "SRC/http.cpp"
  "SRC/MemoryStream.cpp"
  "SRC/pack.cpp"
  "SRC/Scripting.cpp"
  "SRC/tts_voice.cpp"
  "SRC/bl_number_to_words.h"
  "SRC/uni_algo.h"
  "SRC/AES/aes.c"
  "SRC/contextmgr/contextmgr.cpp"
  "SRC/datetime/datetime.cpp"
  "SRC/debugger/debugger.cpp"
  "SRC/fx/freeverb.c"
  "SRC/fx/verblib.h"
  "SRC/scriptany/scriptany.cpp"
  "SRC/scriptarray/scriptarray.cpp"
  "SRC/scriptbuilder/scriptbuilder.cpp"
  "SRC/scriptdictionary/scriptdictionary.cpp"
  "SRC/scriptfile/scriptfile.cpp"
  "SRC/filesystem.cpp"
  "SRC/scriptgrid/scriptgrid.cpp"
  "SRC/scripthandle/scripthandle.cpp"
  "SRC/scripthelper/scripthelper.cpp"
  "SRC/joystick.cpp"
  "SRC/scriptmath/scriptmath.cpp"
  "SRC/scriptmath/scriptmathcomplex.cpp"
  "SRC/scriptmath/scriptmath3d.cpp"
  "SRC/scriptstdstring/scriptstdstring.cpp"
  "SRC/scriptstdstring/scriptstdstring_utils.cpp"
  "SRC/scriptstdstring/scriptstdwstring.cpp"
  "SRC/print_func/print_func.cpp"
  "SRC/obfuscate.h"
  "SRC/IOStreams.cpp"
  "SRC/Graphics.cpp"
)
if(WIN32)
  set(NGT_SOURCES ${NGT_SOURCES} GUI/GUI.cpp)
endif()
add_executable(${PROJECT_NAME} ${NGT_SOURCES})
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION "Release"
)

if(WIN32)
  add_executable(${PROJECT_NAME}W ${NGT_SOURCES})
  add_compile_definitions(${PROJECT_NAME}W NGTW)
  install(TARGETS ${PROJECT_NAME}W
    RUNTIME DESTINATION "Release"
  )
  set_target_properties(${PROJECT_NAME}W PROPERTIES 
    WIN32_EXECUTABLE TRUE      # This sets the subsystem to WINDOWS
)
else()
  add_custom_command(
    TARGET "${PROJECT_NAME}" POST_BUILD
    DEPENDS "${PROJECT_NAME}"
    COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}>
    ARGS --strip-all $<TARGET_FILE:${PROJECT_NAME}>
)
endif()



add_dependencies(${PROJECT_NAME} SDL3)
add_dependencies(${PROJECT_NAME} phonon)
set(SDL_STATIC_DEFAULT ON)

set(SDL_STATIC_AVAILABLE ON)



if(WIN32)
    add_definitions(-DPOCO_STATIC)
    set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} crypt32)
    set(PHONON_LIBRARIES
        ${CMAKE_BINARY_DIR}/phonon-src/lib/windows-x64/phonon.lib
    )
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DELAYLOAD:phonon.dll")
else()
    find_package(Threads REQUIRED)
    set(PHONON_LIBRARIES
            ${CMAKE_BINARY_DIR}/phonon-src/lib/linux-x64/libphonon.so
)
endif()


target_link_libraries(${PROJECT_NAME} SRAL_static)
target_link_libraries(${PROJECT_NAME} angelscript)
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})

if(WIN32)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/external/installed/lib/SDL3-static.lib)
else()
  target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/external/installed/lib/libSDL3.a)
  find_package(X11 REQUIRED)
  target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SpeechD REQUIRED speech-dispatcher)

  target_link_libraries(${PROJECT_NAME} ${SpeechD_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
)



endif()
target_link_libraries(${PROJECT_NAME} Poco::Foundation)
target_link_libraries(${PROJECT_NAME} Poco::Net)
target_link_libraries(${PROJECT_NAME} ${POCO_NETSSL})
target_link_libraries(${PROJECT_NAME} Poco::Util)

target_link_libraries(${PROJECT_NAME} enet)
target_link_libraries(${PROJECT_NAME} ${PHONON_LIBRARIES})
if(WIN32)
  target_link_libraries(${PROJECT_NAME}W SRAL_static)
  target_link_libraries(${PROJECT_NAME}W angelscript)
  target_link_libraries(${PROJECT_NAME}W ${CMAKE_BINARY_DIR}/external/installed/lib/SDL3-static.lib)
  target_link_libraries(${PROJECT_NAME}W Poco::Foundation)
  target_link_libraries(${PROJECT_NAME}W Poco::Net)
  target_link_libraries(${PROJECT_NAME}W ${POCO_NETSSL})
  target_link_libraries(${PROJECT_NAME}W Poco::Util)
  target_link_libraries(${PROJECT_NAME}W enet)
  target_link_libraries(${PROJECT_NAME}W ${PHONON_LIBRARIES})
  target_link_libraries(${PROJECT_NAME} winmm.lib dbghelp.lib)
  target_link_libraries(${PROJECT_NAME} imm32.lib)
  target_link_libraries(${PROJECT_NAME}W winmm.lib dbghelp.lib)
  target_link_libraries(${PROJECT_NAME}W imm32.lib)

  target_link_libraries(${PROJECT_NAME} msvcrt.lib)
  target_link_libraries(${PROJECT_NAME}W msvcrt.lib)

  target_link_libraries(${PROJECT_NAME}
        version.lib
        setupapi.lib
        libcmt.lib
        ws2_32.lib
        iphlpapi.lib
)
target_link_libraries(${PROJECT_NAME}W
        version.lib
        setupapi.lib
        libcmt.lib
        ws2_32.lib
        iphlpapi.lib
)
  endif()
if(MSVC)
  target_link_libraries(${PROJECT_NAME} delayimp.lib)
  target_link_libraries(${PROJECT_NAME}W delayimp.lib)

endif()
