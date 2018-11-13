cmake_minimum_required(VERSION 2.8)

project(GameClient)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(../GameEngine/include)

link_directories(${CMAKE_SOURCE_DIR}/ext/boost/libs)

file(GLOB SRC_LIST "src/*.cpp" "src/*.c")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
find_package(OpenGL REQUIRED)



#find_package(Boost 1.58.0 COMPONENTS filesystem thread) 

#if(Boost_FOUND)
	include_directories(${CMAKE_SOURCE_DIR}/ext/boost/include) 
        add_executable(${PROJECT_NAME} ${SRC_LIST})
        target_link_libraries(${PROJECT_NAME} /home/lawliet/Documents/MainProject/GameEngine/libGameEngine.a ${OPENGL_gl_LIBRARY}  SDL2  GLEW  SOIL libBox2D.a libboost_filesystem.a libboost_system.a libboost_thread.a)
#endif()
