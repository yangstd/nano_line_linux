// test_nano_line.cpp: 定义应用程序的入口。
// 包含调用与结构相关的代码

#include "nano_line.h"
#include <vector>

// the third floor： Design the main function
int main(int argc, const char *argv[])
{
    std::vector<ParamNanoLine> vec_param_nano_line;
    if (!NanoLine::findDevices(vec_param_nano_line))
        return -1;

    if (vec_param_nano_line.empty())
        return -1;

    ParamNanoLine param_nano_line = vec_param_nano_line[0];

    NanoLine nano_line;
    if (!nano_line.init(param_nano_line))
        return -1;
    nano_line.start();

    cv::Mat img;
    while (true)
    {
        nano_line.getData(img);
        if (img.empty())
            continue;
        cv::imshow("demo", img);
        cv::waitKey(10);
    }

    return 0;
}
