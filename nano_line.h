// nano_line.h : 包含结构声明和使用这些结构的函数的原型。

#pragma once

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。

#include "stdio.h"

#ifndef OPENCVINCFLAG
#define OPENCVINCFLAG
#include "opencv2/opencv.hpp"
#endif

#include "cordef.h"
#include "GenApi.h" //!< GenApi lib definitions.
#include "gevapi.h" //!< GEV lib definitions.

#ifdef __cplusplus
extern "C"
{
#include "SapX11Util.h"
#include "X_Display_utils.h"
#include "FileUtil.h"
#include "sched.h"
}
#endif

#include "SapX11Util.h"
#include "X_Display_utils.h"
#include "FileUtil.h"
#include "sched.h"

#include <thread>
#include <vector>

#define PATH_CONFIG "../config.txt"

#define NUM_BUF 8

#define MAX_NETIF 8
#define MAX_CAMERAS_PER_NETIF 32
#define MAX_CAMERAS (MAX_NETIF * MAX_CAMERAS_PER_NETIF)

typedef struct tagMY_CONTEXT
{
    X_VIEW_HANDLE View;
    GEV_CAMERA_HANDLE camHandle;
    int depth;
    int format;
    void *convertBuffer;
    BOOL convertFormat;
    BOOL exit;
} MY_CONTEXT, *PMY_CONTEXT;

struct ParamNanoLine
{

    GEV_DEVICE_INTERFACE nano_line_interface = {0};
    GEV_CAMERA_HANDLE handle = NULL;

    UINT32 height = 0;
    UINT32 width = 0;
    UINT32 format = 0;

    UINT64 payload_size;
    int numBuffers = NUM_BUF;
    PUINT8 bufAddress[NUM_BUF] = {nullptr};

    std::string file_config = PATH_CONFIG;
};

// fourth floor : design class and header files
class NanoLine
{
public:
    NanoLine();
    ~NanoLine();
    static bool findDevices(std::vector<ParamNanoLine> &vec_param_nano_line);
    bool init(const ParamNanoLine &param_nano_line_in);
    // fifth floor: design interface
    bool start();
    bool stop();
    bool getData(cv::Mat &img);

private:
    void ImageTakeThread();

private:
    ParamNanoLine param_nano_line;

    GEV_STATUS status;
    GEV_BUFFER_OBJECT *_img = NULL;

    std::thread tid;
    bool tid_flag = false;
    UINT32 pixFormat = 0;
    UINT32 pixDepth;
    UINT32 convertedGevFormat = 0;
    UINT32 maxHeight = 1600;
    UINT32 maxWidth = 2048;
    UINT32 maxDepth = 2;
};
