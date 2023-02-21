#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>


#define CHRDEV_BASE_MAJOR 100   //主设备号
#define CHRDEV_BASE_NAME  "chrdev_test"  //name


static int chrdev_open(struct inode *pnode, struct file *pfile)
{
    //printk("char dev open! \r\n");
    return 0;
}

static ssize_t chrdev_read(struct file *pfile, char __user *buffer, size_t count, loff_t *ppos)
{
    int ret = 0;
    char *drv_buffer = "driver data!";
    ret = copy_to_user(buffer, drv_buffer, count);
    if(ret == 0)
    {

    }else
        printk("copy to user failed \r\n");
    //printk("char dev read! \r\n");
    return 0;
}

static ssize_t chrdev_write(struct file *pfile,const char __user *buffer, size_t count, loff_t *ppos)
{
    int ret = 0;
    char drv_buffer[100];
    memset(drv_buffer, 0x0, sizeof(drv_buffer));
    ret = copy_from_user(drv_buffer, buffer, count);
    if(ret == 0)
        printk("read from user: %s \r\n",drv_buffer);
    else 
        printk("copy from user failed \r\n");
    //printk("char dev write ! \r\n");
    return 0;
}

static int chrdev_release(struct inode *pnode, struct file *pfile)
{
    //printk("char dev release! \r\n");
    return 0;
}

static struct file_operations ops = {
    .owner = THIS_MODULE,
    .open = chrdev_open,
    .release = chrdev_release,
    .read = chrdev_read,
	.write = chrdev_write,
};

static int __init chrdev_base_init(void)
{
    int ret = 0;
    printk("char dev base init! \r\n");

    ret = register_chrdev(CHRDEV_BASE_MAJOR,CHRDEV_BASE_NAME,&ops);     //注册字符设备
    if(ret < 0)
    {
        printk("register char dev base failed! \r\n");
    }
    return 0;
}

static void __exit chrdev_base_exit(void)
{
    unregister_chrdev(CHRDEV_BASE_MAJOR, CHRDEV_BASE_NAME);     //卸载字符设备
    printk("char dev base exit ! \r\n");
}


module_init(chrdev_base_init);
module_exit(chrdev_base_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangzl");