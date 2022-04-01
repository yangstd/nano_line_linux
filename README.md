# 2022/03/25

* [x] 获得图像数据与设置图像数据 加锁
* [x] 相机配置参数 不依赖外部文件
* [x] 实现断线重连
* [x] 指定若干个相机设备名，
    * [ ] 只采集这些指定设备名的数据
    * [ ] 当有设备未连接时，不管，后台自动断线重连
* [ ] 最终效果：在 main 函数里设置好待获取的设备名，后面采集类会自动重连并获取数据。

* Note: 设备搜索分为两类：
    * 一类是已经搜索到的，就由对象内部负责处理断线重连
    * 一类是以前从未搜到的，新上线的