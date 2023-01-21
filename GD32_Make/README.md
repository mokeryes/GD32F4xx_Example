# GD32_Make
用Make工具编译，openocd工具烧录

## 工程结构
GD32_Make \
├── build 编译完成的工程文件 \
├── Core MCU核心代码文件 \
│   ├── Inc MCU核心代码头文件 \
│   └── Src MCU核心代码源文件 \
├── Docs 工程文档 \
├── Drivers MCU驱动文件 \
│   ├── CMSIS MCU架构文件 \
│   └── GD32F4xx_standard_peripheral MCU驱动文件 \
└── Hardware 用户硬件驱动文件\
    ├── Inc 硬件驱动头文件 \
    └── Src 硬件驱动源代码文件

## 添加源文件需修改的项
添加源文件之后，需要修改**Makefile**，在**C_SOURCES**末端追加头文件路径。 \
若要自己添加头文件目录，需要在**C_INCLUDES**添加新的头文件路径。
