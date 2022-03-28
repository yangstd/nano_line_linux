// nano_line.cpp: 定义应用程序的方法。
// 包含于结构相关的函数的代码。

#include "nano_line.h"

// TODO: 在此处实现功能。

#define NUM_CAMERAS (3)

#define ENABLE_BAYER_CONVERSION 1

#define USE_SYNCHRONOUS_BUFFER_CYCLING 0

#define TUNE_STREAMING_THREADS 0

NanoLine::NanoLine()
{
}

NanoLine::~NanoLine()
{
    stop();
}

bool NanoLine::findDevices(std::vector<ParamNanoLine> &vec_param_nano_line)
{
    vec_param_nano_line.clear();

    int numCamera = 0;
    GEV_STATUS status = 0;
    GEV_DEVICE_INTERFACE pCamera[MAX_CAMERAS] = {0};

    status = GevGetCameraList(pCamera, MAX_CAMERAS, &numCamera);
    std::printf("%d camera(s) on the network\n", numCamera);

    if (status)
    {
        std::printf("nano line find device failed \n");
        return false;
    }

    ParamNanoLine param_nano_line;

    for (uint16_t i = 0; i < numCamera; i++)
    {
        param_nano_line.device_name = pCamera[i].serial;

        param_nano_line.nano_line_interface = pCamera[i];

        vec_param_nano_line.emplace_back(param_nano_line);
    }

    return true;
}

bool NanoLine::init(const ParamNanoLine &param_nano_line_in)
{
    param_nano_line = param_nano_line_in;

    std::printf("%s.\n", param_nano_line.device_name.c_str());

    if (!initOpenCamera())
    {
        std::printf("Open the Camera error! \n");
        return false;
    }

    if (!initInterfaceOptions())
    {
        std::printf("Set the Camera Interface Options error! \n");
        return false;
    }

    if (!initImageParameters())
    {
        std::printf("Get the Camper Image Parameters error! \n");
        return false;
    }

    if (!initTransfer())
    {
        std::printf("Set the Camera Initialize Transfer error!\n");
        return false;
    }

    if (!initStartTransfer())
    {
        std::printf("initStartTransfer error!\n");
        return false;
    }

    return true;
}

void NanoLine::ImageTakeThread()
{
    // While we are still running.
    // std::unique_lock<std::mutex> lock(mtx);
    while (tid_flag)
    {
        // Wait for images to be received
        status = GevWaitForNextImage(param_nano_line.handle, &_img, 1000);

        if (status)
        {
            std::printf("timeout...\n");
            GevStopTransfer(param_nano_line.handle);
            GevAbortTransfer(param_nano_line.handle);
            GevFreeTransfer(param_nano_line.handle);
            GevSetImageParameters(param_nano_line.handle, maxWidth, maxHeight, param_nano_line.x_offset, param_nano_line.y_offset, param_nano_line.format);
            if (!initOpenCamera())
            {
                std::printf("Open the Camera error! \n");
                continue;
            }

            if (!initInterfaceOptions())
            {
                std::printf("Set the Camera Interface Options error! \n");
                continue;
            }

            if (!initImageParameters())
            {
                std::printf("Get the Camper Image Parameters error! \n");
                continue;
            }

            if (!initTransfer())
            {
                std::printf("Set the Camera Initialize Transfer error!\n");
                continue;
            }

            if (!initStartTransfer())
            {
                std::printf("initStartTransfer error!\n");
                continue;
            }
        }
    }
}

bool NanoLine::start()
{
    tid_flag = true;
    tid = std::thread(&NanoLine::ImageTakeThread, this);

    return true;
}

bool NanoLine::stop()
{
    if (tid_flag == true)
    {
        if (tid.joinable())
        {
            tid.join();
            tid_flag = false;
        }
    }
    if (param_nano_line.handle != NULL)
    {
        GevStopTransfer(param_nano_line.handle);
        GevAbortTransfer(param_nano_line.handle);
        GevFreeTransfer(param_nano_line.handle);
        GevCloseCamera(&param_nano_line.handle);
    }
    for (uint16_t i = 0; i < numBuffers; i++)
    {
        if (bufAddress[i] != NULL)
            free(bufAddress[i]);
    }
    printf("cam stop get data!\n");
    GevApiUninitialize();
    _CloseSocketAPI();

    return true;
}

bool NanoLine::getData(cv::Mat &img)
{
    std::unique_lock<std::mutex> lock(mtx);

    if (_img != NULL && status == GEVLIB_OK)
    {
        if (_img->state == 0)
        {
            img = cv::Mat(_img->h, _img->w, CV_8UC1, _img->address);
        }
    }

    return true;
}

bool NanoLine::initOpenCamera()
{
    status = GevOpenCamera(&param_nano_line.nano_line_interface, GevExclusiveMode, &param_nano_line.handle);

    if (status != 0)
    {
        std::printf("Error GevOpenCamera - 0x%x  or %d\n", status, status);
        return false;
    }
    return true;
}

bool NanoLine::initInterfaceOptions()
{
    if (status == 0)
    {
        DALSA_GENICAM_GIGE_REGS reg = {0};
        GEV_CAMERA_OPTIONS camOptions = {0};
        GevGetCameraInterfaceOptions(param_nano_line.handle, &camOptions);
        camOptions.heartbeat_timeout_ms = 5000; // Disconnect detection (5 seconds)
#if TUNE_STREAMING_THREADS
        // Some tuning can be done here. (see the manual)
        camOptions.streamFrame_timeout_ms = 1001;           // Internal timeout for frame reception.
        camOptions.streamNumFramesBuffered = 4;             // Buffer frames internally.
        camOptions.streamMemoryLimitMax = 64 * 1024 * 1024; // Adjust packet memory buffering limit.
        camOptions.streamPktSize = 9180;                    // Adjust the GVSP packet size.
        camOptions.streamPktDelay = 10;                     // Add usecs between packets to pace arrival at NIC.
        // Assign specific CPUs to threads (affinity) - if required for better performance.
        {
            int numCpus = _GetNumCpus();
            if (numCpus > 1)
            {
                camOptions.streamThreadAffinity = numCpus - 1;
                camOptions.serverThreadAffinity = numCpus - 2;
            }
        }
#endif
        // Write the adjusted interface options back.
        GevSetCameraInterfaceOptions(param_nano_line.handle, &camOptions);

        //=================================================================
        // Get the camera registers data structure
        GevGetCameraRegisters(param_nano_line.handle, &reg, sizeof(reg));

        //=================================================================
        // Set up a grab/transfer from this camera
        //
    }
    else
    {
        std::printf("Error initInterfaceOptions - 0x%x  or %d\n", status, status);
        return false;
    }

    return true;
}

bool NanoLine::initImageParameters()
{
    status = GevGetImageParameters(param_nano_line.handle, &param_nano_line.width, &param_nano_line.height, &param_nano_line.x_offset, &param_nano_line.y_offset, &param_nano_line.format);
    if (status == 0)
    {

        std::printf("Camera ROI set for \n\theight = %d\n\twidth = %d\n\tx offset = %d\n\ty offset = %d, pixel format = 0x%08x\n", param_nano_line.height, param_nano_line.width, param_nano_line.x_offset, param_nano_line.y_offset, param_nano_line.format);

        maxHeight = param_nano_line.height;
        maxWidth = param_nano_line.width;
        maxDepth = GetPixelSizeInBytes(param_nano_line.format);

        // Allocate image buffers
        size = maxDepth * maxWidth * maxHeight;
        for (uint16_t i = 0; i < numBuffers; i++)
        {
            bufAddress[i] = (PUINT8)malloc(size);
            memset(bufAddress[i], 0, size);
        }
    }
    else
    {
        std::printf("Error GevGetImageParametersr - 0x%x  or %d\n", status, status);
        return false;
    }

    return true;
}

bool NanoLine::initTransfer()
{
#if USE_SYNCHRONOUS_BUFFER_CYCLING
    // Initialize a transfer with synchronous buffer handling.
    status = GevInitializeTransfer(param_nano_line.handle, SynchronousNextEmpty, size, numBuffers, bufAddress);
#else
    // Initialize a transfer with asynchronous buffer handling.
    status = GevInitializeTransfer(param_nano_line.handle, Asynchronous, size, numBuffers, bufAddress);

    if (status != 0)
    {
        std::printf("Error GevInitializeTransfer - 0x%x  or %d\n", status, status);
        return false;
    }

#endif
    return true;
}

bool NanoLine::initStartTransfer()
{
    for (uint16_t i = 0; i < numBuffers; i++)
    {
        memset(bufAddress[i], 0, size);
    }

    status = GevStartTransfer(param_nano_line.handle, -1);

    if (status != 0)
    {
        std::printf("Error starting grab - 0x%x  or %d\n", status, status);
        return false;
    }

    return true;
}
