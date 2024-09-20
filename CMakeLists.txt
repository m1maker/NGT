# CMakeList.txt : CMake project for NGT
# project specific logic here.
#
cmake_minimum_required (VERSION 3.29.5)
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
include(FetchContent)
FetchContent_Declare(
  angelscript
  GIT_REPOSITORY "https://github.com/codecat/angelscript-mirror"
  GIT_TAG "f2034769ce887367e97a5fbaced5b14aa8039fd3"
  SOURCE_SUBDIR sdk/angelscript/projects/cmake SYSTEM
)
FetchContent_MakeAvailable(angelscript)

include(ExternalProject)
ExternalProject_Add(SDL3
    PREFIX "${CMAKE_BINARY_DIR}/external"
    GIT_REPOSITORY    "https://github.com/libsdl-org/SDL"
    GIT_TAG           "6771a6020da5052e22930c36cb1070a4c048ba3c"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/installed
    )
set_target_properties(SDL3 PROPERTIES EXCLUDE_FROM_ALL TRUE)





FetchContent_Declare(SRAL
  GIT_REPOSITORY "https://github.com/m1maker/SRAL"
  GIT_TAG origin/main SYSTEM)
FetchContent_MakeAvailable(SRAL)
FetchContent_Declare(Poco
  GIT_REPOSITORY "https://github.com/pocoproject/poco"
  GIT_TAG "cd06553d89aee4bf7b5c820199121cd25d5bce94" SYSTEM)
FetchContent_MakeAvailable(Poco)

FetchContent_Declare(enet
  GIT_REPOSITORY "https://github.com/lsalzman/enet"
  GIT_TAG origin/master SYSTEM)
FetchContent_MakeAvailable(enet)


include_directories(
  ${angelscript_SOURCE_DIR}/sdk/angelscript/include
  ${Poco_SOURCE_DIR}/Net/include
  ${Poco_SOURCE_DIR}/Foundation/include

  ${enet_SOURCE_DIR}/include
  ${SRAL_SOURCE_DIR}/Include
  ${CMAKE_BINARY_DIR}/external/installed/include

)



add_executable(${PROJECT_NAME} 
  "SRC/ngt.cpp" 
  "SRC/ngt.h" 
  "SRC/main.cpp"
  "SRC/ngtreg.h"
  "SRC/ngtreg.cpp"
  "SRC/sound.h"
  "SRC/sound.cpp"
)


add_dependencies(${PROJECT_NAME} SDL3)


target_link_libraries(${PROJECT_NAME} SRAL_static)
target_link_libraries(${PROJECT_NAME} angelscript)
target_link_libraries(${PROJECT_NAME} SDL_STATIC)
target_link_libraries(${PROJECT_NAME} PocoFoundation)
target_link_libraries(${PROJECT_NAME} PocoNet)
target_link_libraries(${PROJECT_NAME} enet)
