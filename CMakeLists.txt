cmake_minimum_required(VERSION 3.20)
project("TMC" C CXX ASM)
include(cmake/st-project.cmake)


add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})   #ADD NECESSARY COMPILE FLAGS



# get_target_property(TARGET_INCL ${PROJECT_NAME} INCLUDE_DIRECTORIES)

#ADD SRC FROM DIRECTORY NAMED LIKEPROJECT
add_subdirectory(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${MAIN_SOURCES})