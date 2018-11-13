cmake_minimum_required(VERSION 2.8)

project(GameClient)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(../GameEngine/include)


file(GLOB SRC_LIST "src/*.cpp" "src/*.c")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
find_package(OpenGL REQUIRED)

find_package(Boost 1.58.0 COMPONENTS filesystem thread) 

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS}) 
        add_executable(${PROJECT_NAME} ${SRC_LIST})
        target_link_libraries(${PROJECT_NAME} /home/lawliet/Documents/MainProject/GameEngine/libGameEngine.a /home/lawliet/Documents/MainProject/nanogui/libnanogui.so ${OPENGL_gl_LIBRARY} ${Boost_LIBRARIES}  SDL2  GLEW  SOIL libBox2D.a)
endif()
