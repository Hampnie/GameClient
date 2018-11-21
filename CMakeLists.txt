cmake_minimum_required(VERSION 2.8)

project(GameClient)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(../GameEngine/include)

include_directories(${CMAKE_SOURCE_DIR}/ext/boost/include)
include_directories(${CMAKE_SOURCE_DIR}/ext/Box2D/include)
include_directories(${CMAKE_SOURCE_DIR}/ext/glm/include)
include_directories(${CMAKE_SOURCE_DIR}/ext/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/ext/SOIL/include)

link_directories(${CMAKE_SOURCE_DIR}/ext/boost/libs)
link_directories(${CMAKE_SOURCE_DIR}/ext/Box2D/libs)
link_directories(${CMAKE_SOURCE_DIR}/ext/SDL/libs)
link_directories(${CMAKE_SOURCE_DIR}/ext/SOIL/libs)

file(GLOB SRC_LIST "src/*.cpp" "src/*.c")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
find_package(OpenGL REQUIRED)


	 
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} /home/lawliet/Documents/MainProject/GameEngine/libGameEngine.a ${OPENGL_gl_LIBRARY}  SDL2  GLEW  SOIL libBox2D.a boost_system boost_filesystem boost_thread)

