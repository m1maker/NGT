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
  SOURCE_SUBDIR sdk/angelscript/projects/cmake
)
FetchContent_MakeAvailable(angelscript)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
  GIT_TAG origin/main
)
FetchContent_MakeAvailable(SDL3)



FetchContent_Declare(SRAL
  GIT_REPOSITORY "https://github.com/m1maker/SRAL"
  GIT_TAG origin/main)
FetchContent_MakeAvailable(SRAL)
FetchContent_Declare(Poco
  GIT_REPOSITORY "https://github.com/pocoproject/poco"
  GIT_TAG origin/main)
FetchContent_MakeAvailable(Poco)


add_executable(${PROJECT_NAME} 
  "SRC/ngt.cpp" 
  "SRC/ngt.h" 
  "SRC/main.cpp")
target_link_libraries(${PROJECT_NAME} SRAL_static)
target_link_libraries(${PROJECT_NAME} angelscript)
target_link_libraries(${PROJECT_NAME} SDL_STATIC)
target_link_libraries(${PROJECT_NAME} PocoFoundation)
