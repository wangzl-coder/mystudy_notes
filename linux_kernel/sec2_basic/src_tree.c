#if 0
    arch 特定体系结构源码
    block 块设备IO层
    crypto 加密API
    Documentation 内核源码文档
    drivers 设备驱动程序
    firmware 某些驱动程序需要使用得设备固件
    fs VFS和各种文件系统
    include 内核头文件
    init 内核引导和初始化
    ipc 进程间通信代码
    kernel 核心子系统（进程调度等）
    lib 通用内核函数
    mm 内存管理子系统和VM
    net 网络子系统
    samples 示例代码
    scripts 编译内核使用得脚本
    security linux安全模块
    sound 语音子系统
    usr 早期用户空间代码（initramfs）
    tools linux开发中有用得工具
    virt 虚拟化基础结构

    编译：
        make config -> 逐一遍历配置项
        make menuconfig -> 基于ncurse库得图形化配置
        make gconfig -> 基于gtk+得图形工具
        make defconfig -> 基于默认得配置
        make oldconfig -> 根据.config文件重新配置代码树
        make > /dev/null -> 无用信息到黑洞
        make -jn -> 多个作业编译内核
        make module_install -> 模块安装
        符号对照表 -> System.map
#endif
