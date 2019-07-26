cmake_minimum_required(VERSION 2.8)
project( facenet )

SET(CMAKE_C_COMPILER arm-buildroot-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-buildroot-linux-gnueabihf-c++)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

aux_source_directory(./src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories( ../ncnn/build/src )
include_directories( ../ncnn/src )

#find_package( OpenCV REQUIRED )
#include_directories( ${OpenCV_INCLUDE_DIRS} )
#-ljpeg -lcurl -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lyuv -lzbar

target_link_libraries(facenet /media/user1/V3SSDK_BUILDROOT/v3ssdk/app/ncnn/build/src/libncnn.a)
target_link_libraries(facenet -lopencv_core)
target_link_libraries(facenet -lopencv_imgproc)
target_link_libraries(facenet -lopencv_videoio)
target_link_libraries(facenet -lopencv_imgcodecs)
