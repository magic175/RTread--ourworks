    # RTread--ourworks
    # RT-Thread LED 呼吸灯与按键报警系统
    基于 RT-Thread 设计一个 LED 呼吸灯与按键报警系统，使用多线程调度实现 LED 呼吸、按键检测、报警处理三个相互独立的功能线程，并通过消息队列、信号量、互斥量完成线程同步与通信。

    - `main`：稳定版本。
    - `driver-main`：队员 A 的硬件驱动和主控框架开发分支。
    - `thread-function`：队员 B 的功能线程和通信逻辑开发分支。


    # 最终模块说明

    - `applications/main.c`：系统入口，初始化硬件层和线程层。
    - `applications/driver_layer.c/.h`：LED、按键、蜂鸣器硬件抽象接口。
    - `applications/thread_function.c/.h`：按键扫描线程、LED 呼吸线程、报警线程，以及消息队列、信号量、互斥量。

    ## 预期串口现象

    启动后串口输出系统初始化日志；LED 周期性呈现呼吸灯效果；短按按键后蜂鸣器报警，长按按键后取消报警并复位 LED。    
