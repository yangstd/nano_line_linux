# 设计一个类

``` cpp
/*
 - x (工程文件夹)
     - x.h (类的声明) 
     - x.cpp (类的定义)
     - test_x.cpp (顶层测试文件)

struct ParamX; (X 的数据)
class X (X 的类声明方式)
{
    bool init(const ParamX& param_x_in);
    void start();

    ParamX param_x;
}


*/



/* TODO
- ParamNaneLine
- findDevices
- NanoLine
- init
- start
- getData
*/
```

```cpp
test_nano_line
├─ .vscode
│  └─ settings.json
├─ CMakeLists.txt
├─ README.md
├─ build
│  ├─ CMakeCache.txt
│  ├─ CMakeFiles
│  │  ├─ 3.10.2
│  │  │  ├─ CMakeCXXCompiler.cmake
│  │  │  ├─ CMakeDetermineCompilerABI_CXX.bin
│  │  │  ├─ CMakeSystem.cmake
│  │  │  └─ CompilerIdCXX
│  │  │     ├─ CMakeCXXCompilerId.cpp
│  │  │     ├─ a.out
│  │  │     └─ tmp
│  │  ├─ CMakeOutput.log
│  │  ├─ CMakeTmp
│  │  ├─ TargetDirectories.txt
│  │  ├─ cmake.check_cache
│  │  ├─ feature_tests.bin
│  │  ├─ feature_tests.cxx
│  │  └─ test_nano_line.dir
│  ├─ build.ninja
│  ├─ cmake_install.cmake
│  ├─ compile_commands.json
│  └─ rules.ninja
├─ common
│  ├─ FileUtil.h
│  ├─ FileUtil_tiff.c
│  ├─ GevFileUtils.c
│  ├─ GevUtils.c
│  ├─ SapExUtil.c
│  ├─ SapExUtil.h
│  ├─ SapX11Util.h
│  ├─ X_Display_utils.c
│  ├─ X_Display_utils.h
│  ├─ commondefs.mk
│  └─ convertBayer.c
├─ nano_line.cpp
├─ nano_line.h
└─ test_nano_line.cpp
```
