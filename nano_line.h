// nano_line.h : 包含结构声明和使用这些结构的函数的原型。

#pragma once

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。

#ifndef OPENCVINCFLAG
#define OPENCVINCFLAG
#include "opencv2/opencv.hpp"
#endif

#include "cordef.h"
#include "GenApi.h" //!< GenApi lib definitions.
#include "gevapi.h" //!< GEV lib definitions.
#include "SapX11Util.h"
#include "X_Display_utils.h"
#include "sched.h"

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#define MAX_NETIF 8
#define MAX_CAMERAS_PER_NETIF 32
#define MAX_CAMERAS (MAX_NETIF * MAX_CAMERAS_PER_NETIF)

#define USE_SYNCHRONOUS_BUFFER_CYCLING 0

#define NUM_BUF 8

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
    std::string device_name;

    uint32_t width;
    uint32_t height;
    uint32_t x_offset;
    uint32_t y_offset;
    uint32_t format;

    GEV_DEVICE_INTERFACE nano_line_interface;
    GEV_CAMERA_HANDLE handle;
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
    bool initOpenCamera();
    bool initInterfaceOptions();
    bool initImageParameters();
    bool initTransfer();
    bool initStartTransfer();
    void ImageTakeThread();

private:
    ParamNanoLine param_nano_line;

    GEV_STATUS status = 0;
    GEV_BUFFER_OBJECT *_img = NULL;

    uint32_t maxHeight = 1920;
    uint32_t maxWidth = 1080;
    uint32_t maxDepth = 2;
    uint64_t size = 0;

    uint64_t payload_size = 0;
    uint32_t numBuffers = NUM_BUF;
    PUINT8 bufAddress[NUM_BUF];

    std::thread tid;
    bool tid_flag = false;
    std::mutex mtx;
    std::condition_variable cva;
};
