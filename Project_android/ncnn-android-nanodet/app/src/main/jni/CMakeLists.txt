project(nanodetncnn)

cmake_minimum_required(VERSION 3.10)

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv-mobile-4.5.1-android/sdk/native/jni)
find_package(OpenCV REQUIRED core imgproc)

set(ncnn_DIR ${CMAKE_SOURCE_DIR}/ncnn-20210525-android-vulkan/${ANDROID_ABI}/lib/cmake/ncnn)
find_package(ncnn REQUIRED)


file(GLOB_RECURSE native_srcs *.cpp)

add_library(nanodetncnn SHARED ${native_srcs})

#add_library(nanodetncnn SHARED nanodetncnn.cpp nanodet.cpp ndkcamera.cpp)

target_link_libraries(nanodetncnn ncnn ${OpenCV_LIBS} camera2ndk mediandk)
