cmake_minimum_required(VERSION 3.5)
project(nanogui_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(lib/nanogui)
include_directories(lib/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

include_directories(lib/embed-resource)
add_subdirectory(lib/embed-resource)

embed_resources(MyResources shader/vert.glsl shader/frag.glsl)

# add_executable(mytest test.cpp ${MyResources})
# target_link_libraries(mytest nanogui ${NANOGUI_EXTRA_LIBS})

set(OBJ_SOURCE_FILES
    obj_view.cpp
    MeshMcd.cpp
    MeshMcd.h
    MeshSd.cpp
    MeshSd.h
    Mesh.cpp
    Mesh.h
    W_edge.cpp
    W_edge.h)

add_executable(mcaq ${OBJ_SOURCE_FILES} ${MyResources})
target_link_libraries(mcaq nanogui ${NANOGUI_EXTRA_LIBS})

