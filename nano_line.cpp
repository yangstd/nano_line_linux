// nano_line.cpp: 定义应用程序的方法。
// 包含于结构相关的函数的代码。

#include "nano_line.h"

// TODO: 在此处实现功能。

#define NUM_CAMERAS (3)

#define ENABLE_BAYER_CONVERSION 1

NanoLine::NanoLine()
{
}

NanoLine::~NanoLine()
{
    stop();
}

/**
 * @brief
 *
 * @param vec_param_nano_line
 * @return true
 * @return false
 */
bool NanoLine::findDevices(std::vector<ParamNanoLine> &vec_param_nano_line)
{
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

    vec_param_nano_line.clear();

    ParamNanoLine param_nano_line;

    for (uint16_t i = 0; i < numCamera; i++)
    {

        param_nano_line.nano_line_interface = pCamera[i];

        vec_param_nano_line.emplace_back(param_nano_line);
    }

    return true;
}

bool NanoLine::init(const ParamNanoLine &param_nano_line_in)
{
    int type = 0;
    UINT64 size = 0;
    param_nano_line = param_nano_line_in;

    status = GevOpenCamera(&param_nano_line.nano_line_interface, GevExclusiveMode, &param_nano_line.handle);

    if (status == 0)
    {
        char xmlFileName[MAX_PATH] = {0};
        status = GevGetGenICamXML_FileName(param_nano_line.handle, (int)sizeof(xmlFileName), xmlFileName);
        if (status == GEVLIB_OK)
        {
            printf("XML stored as %s\n", xmlFileName);
        }
        status = GEVLIB_OK;
    }

    printf("status (xml): %d. \n", status);

    if (status == 0)
    {
        GEV_CAMERA_OPTIONS camOptions = {0};
        GevGetCameraInterfaceOptions(param_nano_line.handle, &camOptions);
        camOptions.heartbeat_timeout_ms = 5000;

        GevSetCameraInterfaceOptions(param_nano_line.handle, &camOptions);
    }
    printf("status (option): %d. \n", status);

    GenApi::CNodeMapRef *Camera = static_cast<GenApi::CNodeMapRef *>(GevGetFeatureNodeMap(param_nano_line.handle));

    if (Camera)
    {
        // Access some features using the bare GenApi interface methods
        try
        {
            // Mandatory features....
            GenApi::CIntegerPtr ptrIntNode = Camera->_GetNode("Width");
            param_nano_line.width = (UINT32)ptrIntNode->GetValue();

            ptrIntNode = Camera->_GetNode("Height");
            param_nano_line.height = (UINT32)ptrIntNode->GetValue();

            ptrIntNode = Camera->_GetNode("PayloadSize");
            param_nano_line.payload_size = (UINT64)ptrIntNode->GetValue();

            GenApi::CEnumerationPtr ptrEnumNode = Camera->_GetNode("PixelFormat");
            param_nano_line.format = (UINT32)ptrEnumNode->GetIntValue();
        }
        // Catch all possible exceptions from a node access.
        CATCH_GENAPI_ERROR(status);
    }

    printf("status (camera): %d. \n", status);

    if (status == 0)
    {
        printf("Camera ROI set for \n \t Height = %d\n\t Width = %d\n\t PixelFormat (val) = 0x%08x\n", param_nano_line.height, param_nano_line.width, param_nano_line.format);

        maxHeight = param_nano_line.height;
        maxWidth = param_nano_line.width;
        maxDepth = GetPixelSizeInBytes(param_nano_line.format);

        size = maxDepth * maxWidth * maxHeight;
        size = (param_nano_line.payload_size > size) ? param_nano_line.payload_size : size;

        for (uint16_t i = 0; i < param_nano_line.numBuffers; i++)
        {

            param_nano_line.bufAddress[i] = (PUINT8)malloc(size);
            memset(param_nano_line.bufAddress[i], 0, size);
        }
    }

    printf("status (size): %d. \n", status);

    status = GevInitializeTransfer(param_nano_line.handle, Asynchronous, size, param_nano_line.numBuffers, param_nano_line.bufAddress);

    printf("status (asyn): %d. \n", status);

    status = GevStartTransfer(param_nano_line.handle, -1);

    printf("status (sfer): %d. \n", status);

    return true;
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
    for (int i = 0; i < param_nano_line.numBuffers; i++)
    {
        if (param_nano_line.bufAddress[i] != NULL)
            free(param_nano_line.bufAddress[i]);
    }
    printf("cam stop get data!\n");
    return true;
}

bool NanoLine::getData(cv::Mat &img)
{
    if (_img != NULL && status == GEVLIB_OK)
    {
        img = cv::Mat(_img->h, _img->w, CV_8UC1, _img->address);
    }

    return true;
}

void NanoLine::ImageTakeThread()
{
    // While we are still running.
    while (tid_flag)
    {
        // Wait for images to be received
        status = GevWaitForNextImage(param_nano_line.handle, &_img, 1000);
        // if (_img != NULL && status == GEVLIB_OK)
        // {
        //     cv::Mat Img = cv::Mat(_img->h, _img->w, CV_8UC1, _img->address);
        //     if (Img.empty())
        //         continue;
        //     cv::imshow("demo", Img);
        //     cv::waitKey(10);
        // }
    }
}