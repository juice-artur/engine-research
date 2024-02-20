cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME engine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

include_directories(src)
include_directories($ENV{VULKAN_SDK}/Include)


file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)


foreach(source IN LISTS SOURCE_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" source_path_fancy "${source_path}")
    source_group("${source_path_fancy}" FILES "${source}")
endforeach()


set(LINKER_FLAGS user32.lib vulkan-1.lib)
link_directories($ENV{VULKAN_SDK}/Lib)

if(ANDROID)
    add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)
elseif(WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif() 

add_compile_definitions(D_DEBUG JE_EXPORT)
set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LINKER_FLAGS})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/../external/SDL2/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/../external/SDL2/lib/x64/SDL2.lib"
                                      "${CMAKE_SOURCE_DIR}/../external/SDL2/lib/x64/SDL2main.lib")

add_subdirectory(../external/glm  ${CMAKE_CURRENT_BINARY_DIR}/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
                                      

add_subdirectory(../external/spdlog  ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/../external/SDL2/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)


set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>
)
