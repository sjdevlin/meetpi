project(meetpi)
cmake_minimum_required(VERSION 2.4.6)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_library(MATRIX_CREATOR_HAL matrix_creator_hal)
find_library(STEPHEN_LIB  food)


include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_BUILD_TYPE "Release")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

#Add base directory for includes (global)
#sd include_directories(include/odas)

#sd add_subdirectory(include)

#sd add_library(odas SHARED
#sd 	${SRC}
#sd )


add_executable(c_test
    ~/meetpi/c_test.c
)

target_link_libraries(c_test
    ${STEPHEN_LIB}
    ${MATRIX_CREATOR_HAL}

)


install(TARGETS c_test DESTINATION bin)
