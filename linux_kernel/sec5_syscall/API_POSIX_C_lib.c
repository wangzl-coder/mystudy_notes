#if 0
    API,POSIX,和C库：

    （1）一般来说，应用程序在用户空间实现的应用编程接口（API）而不是直接通过系统调用来编程。一个API
        可以实现成一个系统调用，也可以通过调用多个系统调用来实现，而完全不使用任何系统调用也不存在问题。

    （2）API在不同的操作系统上给应用程序提供相同的接口，而在实现上可能迥异。

    （3）调用关系：
        a. 例如printf(应用程序)->printf(C库)->write(C库)->write(系统调用)
        b. POSIX：Unix最流行的应用编程接口。提供一套大体上基于Unix的可移植操作系统标准。

    （4）Unix只跟系统调用打交道，怎么使用内核不关系
#endif