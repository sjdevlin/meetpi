project(meetpi)
cmake_minimum_required(VERSION 2.4.6)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_library(MATRIX_CREATOR_HAL matrix_creator_hal)

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_BUILD_TYPE "Release")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

#Add base directory for includes (global)
#sd include_directories(include/odas)

#sd add_subdirectory(include)

find_path (JSON_C_INCLUDE_DIR json.h PATH_SUFFIXES json-c json)
find_library (JSON_C_LIBRARIES NAMES json-c libjson-c)
include_directories (${JSON_C_INCLUDE_DIR})


#sd add_library(odas SHARED
#sd 	${SRC}
#sd )


add_executable(matrix 
    ~/meetpi/Matrix.cpp
)

add_executable(ledpie 
    ~/meetpi/ledpie.cpp
)

add_executable(ledpie_tar 
    ~/meetpi/ledpie_for_targets.cpp
)

add_executable(ledpie_pot 
    ~/meetpi/ledpie_for_pots.cpp
)


target_link_libraries(matrix 
    ${JSON_C_LIBRARIES}
    ${MATRIX_CREATOR_HAL}
)

target_link_libraries(ledpie 
    ${JSON_C_LIBRARIES}
    ${MATRIX_CREATOR_HAL}
)

target_link_libraries(ledpie_tar 
    ${JSON_C_LIBRARIES}
    ${MATRIX_CREATOR_HAL}
)

target_link_libraries(ledpie_pot 
    ${JSON_C_LIBRARIES}
    ${MATRIX_CREATOR_HAL}
)



install(TARGETS matrix DESTINATION bin)
install(TARGETS ledpie DESTINATION bin)
install(TARGETS ledpie_tar DESTINATION bin)
install(TARGETS ledpie_pot DESTINATION bin)
