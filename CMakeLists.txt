# CMakeList.txt : CMake project for NGT
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NGT")
include(FetchContent)
FetchContent_Declare(
  angelscript
  GIT_REPOSITORY "https://github.com/codecat/angelscript-mirror"
  GIT_TAG origin/master
  SOURCE_SUBDIR sdk/angelscript/projects/cmake SYSTEM
)
FetchContent_MakeAvailable(angelscript)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
  GIT_TAG origin/main SYSTEM
)
FetchContent_MakeAvailable(SDL3)



FetchContent_Declare(SRAL
  GIT_REPOSITORY "https://github.com/m1maker/SRAL"
  GIT_TAG origin/main SYSTEM)
FetchContent_MakeAvailable(SRAL)
FetchContent_Declare(Poco
  GIT_REPOSITORY "https://github.com/pocoproject/poco"
  GIT_TAG origin/main SYSTEM)
FetchContent_MakeAvailable(Poco)

FetchContent_Declare(enet
  GIT_REPOSITORY "https://github.com/lsalzman/enet"
  GIT_TAG origin/master SYSTEM)
FetchContent_MakeAvailable(enet)

include_directories(
  ${angelscript_SOURCE_DIR}/sdk/angelscript/include
  ${Poco_SOURCE_DIR}/Net/include
  ${Poco_SOURCE_DIR}/Foundation/include

  ${SDL3_SOURCE_DIR}/include
  ${enet_SOURCE_DIR}/include
  ${SRAL_SOURCE_DIR}/Include

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
target_link_libraries(${PROJECT_NAME} SRAL_static)
target_link_libraries(${PROJECT_NAME} angelscript)
target_link_libraries(${PROJECT_NAME} SDL_STATIC)
target_link_libraries(${PROJECT_NAME} PocoFoundation)
target_link_libraries(${PROJECT_NAME} PocoNet)
target_link_libraries(${PROJECT_NAME} enet)
