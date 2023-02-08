<p align="center">
<img src="docs/images/agloo_400x200.png" width="200" >
</p>


# Agloo

Agloo（中文读作“阿格鲁”）是一个专用于嵌入式 Linux 系统的应用程序开发框架。Agloo 框架包含设备端应用程序常见的模块，包括消息通信、设备管理、数据采集、协议解析、状态监控、配置管理、日志记录、远程升级、图形界面等功能模块。模块之间采用消息通信机制进行同步，天生支持分布式部署，也就是说，功能模块可以部署在不同的硬件平台（包括异构多核平台）。

## 项目起源

我们曾经用 Linux 系统做过许多嵌入式/物联网项目，包括工业控制设备、通信终端、数据采集仪、高精传感器、物联网、车联网等。在多次重复开发之后，觉得有必要将一些通用的功能模块抽象出来，方便大家复用，加速开发更多有趣的项目。因此，我们创建了 Agloo 项目。

Agloo 项目默认实现了 Linux 系统监控功能，可通过 Web 查看系统状态。

## 框架结构



## 模块功能

| 模块名称 | 简介         | 说明                                                         |
| -------- | ------------ | ------------------------------------------------------------ |
| Log      | 日志服务系统 | 抽象日志输入接口，实现日志的统一管理，包括定时冲刷、自动滚动、磁盘管理等。 |
| OTA      | 远程升级     | 包括在线升级、离线升级、状态脚本等功能                       |
| MSG      | 消息服务     | 支持 DBus、MQTT、ZeroMQ、rabbitMQ、DDS 等底层消息服务        |
|          |              |                                                              |
|          |              |                                                              |




## 开发计划

See [TODO](TODO.md)



## 应用案例

- [智能咖啡机]()
- [环保数采仪]()
- [自助洗车机]()
- [食堂称重计费终端]()
- [激光雷达](https://github.com/luhuadong/LidarApp)

