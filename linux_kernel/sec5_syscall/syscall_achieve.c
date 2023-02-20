#if 0
    系统调用的实现
        
        1 实现系统调用：
            （1）明确用途，一个系统调用通过传递不同的参数值来选择完成不同的工作。
            （2）通用性，可移植性和健壮性

        2 参数验证：
            （1）检查参数的合法性，内核不应访问不合法或者无权访问的资源
            （2）指针传参问题，进程不能绕过内存访问限制
            （3）参数的检查和用户空间与内核空间的数据拷贝：
                a. copy_from_user()
                b. copy_to_user()
                @return : success for 0, or -EFAULT for error
            （4）注意：两个函数可能引起阻塞，当包含用户数据的页被换出到硬盘上而不是在物理内存上
                的时候，就会发生。
            （5）capable().检查访问资源的权限
#endif
SYSCALL_DEFINE3(silly_copy, unsigned long *,src, unsigned long *,dst, unsigned long len)
{
    unsigned long buf;
    if(copy_from_user(&buf, src, len))
        return -EFAULT;
    if(copy_to_user(dst, &buf, len))
        return -EFAULT;

    return len;
}

SYS_DEFINE4(reboot, int, magic1, int, magic2, unsigned int, cmd, void __user *, arg)
{
    char buffer[256];
    /*
     * 只信任启动系统的系统管理员
     * */
    if(!capable(CAP_SYS_BOOT))
        return -EPERM;
    /*
     * 为安全起见，需要magic参数
     * */
    if(magic1 != LINUX_REBOOT_MAGIC1 ||
        (magic2 != LINUX_REBOOT_MAGIC2 &&
                magic2 != LINUX_REBOOT_MAGIC2A &&
                magic2 != LINUX_REBOOT_MAGIC2B &&
                magic2 != LINUX_REBOOT_MAGIC2A))
        return -EINVAL;

    /*
     * 当未设置pm_power_off时，请不要试图让power_off的代码看起来像是可以停机，而应该采用
     * 更简单的方式
     * */
    if((cmd == LINUX_REBOOT_CMD_POWER_OFF) && !pm_power_off)
        cmd = LINUX_REBOOT_CMD_HAIT;

    lock_kernel();
    switch(cmd) {
        case LINUX_REBOOT_CMD_RESTART:
            kernel_restart(NULL);
            break;
        case LINUX_REBOOT_CMD_CAD_OFF:
            C_A_D = 1;
            break;
        case LINUX_REBOOT_CMD_HALT:
            kernel_halt();
            unlock_kernel();
            do_exit(0);
            break;
        case LINUX_REBOOT_CMD_POWER_OFF:
            kernel_power_off();
            unlock_kernel();
            do_exit(0);
            break;
        case LINUX_REBOOT_CMD_RESTART2:
            if(strncpy_from_user(&buffer[0], arg, sizeof(buffer) - 1) < 0) {
                unlock_kernel();
                return -EFAULT;
            }
            buffer[sizeof(buffer) - 1] = '\0';

            kernel_restart(buffer);
            break;
        default:
            unlock_kernel();
            return -EINVAL;
    }
    unlock_kernel();
    return 0;
}
