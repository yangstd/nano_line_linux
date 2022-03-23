# 声明要求的 cmake 最低版本
# 终端输入`cmake -version`可查看`cmake`的版本
cmake_minimum_required( VERSION 3.10.0 )

# 定义项目名称变量PROJECT_NAME, 默认值为 本项目的Demo
set( PROJECT_NAME test_nano_line )

# 项目名称以及版本 
project( ${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX )

# 设置 'cmake' 编译模式为 'debug'
# set( CMAKE_BUILD_TYPE Debug )

# 设置C++的版本
# set( CMAKE_CXX_STANDARD 11 )
# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native -std=c++11 ")

# 打印信息
MESSAGE( "Build type:       ${CMAKE_BUILD_TYPE}")
MESSAGE( "CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}")


# 第三方库
# 查找OpenCV

find_package(OpenCV 3.4.1 REQUIRED)
if(OpenCV_FOUND)
  message( "Found OpenCV libraries" )
else()
  message("Can't find libavcodec, libavformat or libavutil. Add them!")
endif()


# 查找Eigen3
# find_package(Eigen3 3.1.0 REQUIRED)
# 查找Pangolin
# find_package(Pangolin REQUIRED)



# FFmpeg
set(FFmpeg_INCLUDE_DIR /opt/ffmpeg/include)
link_directories(/opt/ffmpeg/lib )
set(FFmpeg_LIBRARIES avfilter avcodec avutil swscale avdevice avformat swresample)



# GenICam
set(GenICam_INCLUDE_DIRS /usr/dalsa/GigeV/include /opt/genicam_v3_0/library/CPP/include/GenApi /opt/genicam_v3_0/library/CPP/include)
link_directories(/usr/local/lib /opt/genicam_v3_0/bin/Linux64_x64)
set(GenICam_LIBRARIES pthread GevApi CorW32 GenApi_gcc421_v3_0 GCBase_gcc421_v3_0)



MESSAGE("PROJECT_SOURCE_DIR:    ${PROJECT_SOURCE_DIR}")
MESSAGE("OpenCV_INCLUDE_DIRS:   ${OpenCV_INCLUDE_DIRS}")
MESSAGE("FFmpeg_INCLUDE_DIR:    ${FFmpeg_INCLUDE_DIR}")
MESSAGE("GenICam_INCLUDE_DIRS:  ${GenICam_INCLUDE_DIRS}")


# 设定头文件目录
include_directories(${PROJECT_SOURCE_DIR} 
                    ${PROJECT_SOURCE_DIR}/common
                    ${OpenCV_INCLUDE_DIRS}
                    ${FFmpeg_INCLUDE_DIR}
                    ${GenICam_INCLUDE_DIRS}
)


# 包含 cmake 当前文件夹目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 查找当前目录下的所有源文件
aux_source_directory(. DIR_SRCS)
aux_source_directory(./common RR)


# 设定输出文件生成目录
# 设定生成的库文件的存放目录
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
# MESSAGE("CMAKE_LIBRARY_OUTPUT_DIRECTORY:  ${PROJECT_SOURCE_DIR}/build/lib ")

# 生成静态库
# add_library(${PROJECT_NAME} SHARED)
# ORB_SLAM2: ORB_SLAM2


# 设定生成的二进制文件的存放目录
 set( EXEXUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)
 MESSAGE("EXEXUTABLE_OUTPUT_PATH:  ${PROJECT_SOURCE_DIR}/build ")

# 生成二进制文件
# 添加一个可执行程序, 务必 要放在 link_directories 后面； 要放在 target_link_libraries 前面。
# 语法 : add_executable( 程序名 源代码文件 )
add_executable( ${PROJECT_NAME}  ${DIR_SRCS} ${RR})


# 需要链接的库文件
target_link_libraries(${PROJECT_NAME}
                      ${OpenCV_LIBS}
                      ${FFmpeg_LIBRARIES}
                      ${GenICam_LIBRARIES}
)

