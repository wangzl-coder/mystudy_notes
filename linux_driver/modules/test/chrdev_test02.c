#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>



#define NEW_CHRDEV_BASE_NAME "chrdev_test02"
#define NEW_CHRDEV_BASE_COUNT 1


struct chrbase_dev{
    dev_t devid;        //设备号
    int major;          //主设备号
    int minor;          //次设备号
    struct cdev cdev;   //cdev
    struct class *class;    //类
    struct device *device;  //设备
};

static struct chrbase_dev newChrdev; 

static int newChrdev_open(struct inode *pnode, struct file *pfile)
{
    //printk("char dev open! \r\n");
    return 0;
}

static ssize_t newChrdev_read(struct file *pfile, char __user *buffer, size_t count, loff_t *ppos)
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

static ssize_t newChrdev_write(struct file *pfile,const char __user *buffer, size_t count, loff_t *ppos)
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

static int newChrdev_release(struct inode *pnode, struct file *pfile)
{
    //printk("char dev release! \r\n");
    return 0;
}

static struct file_operations newChrOps = {
    .open = newChrdev_open,
    .release = newChrdev_release,
    .read = newChrdev_read,
    .write = newChrdev_write,
};


static int __init chrdev_base_init(void)
{
    int ret = 0;
    if(newChrdev.major){     /** 给定主设备号,注册设备号 **/
        newChrdev.devid = MKDEV(newChrdev.major, 0);
        ret = register_chrdev_region(newChrdev.devid, NEW_CHRDEV_BASE_COUNT, NEW_CHRDEV_BASE_NAME);
    } else {                /** 未给定主设备号,申请设备号 **/
        ret = alloc_chrdev_region(&newChrdev.devid, 0, NEW_CHRDEV_BASE_COUNT, NEW_CHRDEV_BASE_NAME);
        newChrdev.major = MAJOR(newChrdev.devid);
        newChrdev.minor = MINOR(newChrdev.devid);
    }
    if(ret < 0){
        printk("chrdev chrdev_region error! \r\n");
        goto devid_failed;
    }
    printk("chrdev major=%d, minor=%d \r\n", newChrdev.major, newChrdev.minor);

    /** 注册字符设备 **/
    newChrdev.cdev.owner = THIS_MODULE;
    cdev_init(&newChrdev.cdev, &newChrOps);
    ret = cdev_add(&newChrdev.cdev, newChrdev.devid, NEW_CHRDEV_BASE_COUNT);
    if(ret < 0){
        printk("chrdev add failed \r\n");
        goto dev_add_filed;
    }

    /** 自动创建设备节点 **/
    newChrdev.class = class_create(newChrdev.cdev.owner, NEW_CHRDEV_BASE_NAME);     //创建类
    if(IS_ERR(newChrdev.class)){
        ret = PTR_ERR(newChrdev.class);
        goto class_failed;
    }

    newChrdev.device = device_create(newChrdev.class, NULL, newChrdev.devid, NULL, NEW_CHRDEV_BASE_NAME);
    if(IS_ERR(newChrdev.device)){
        ret = PTR_ERR(newChrdev.device);
        goto device_failed;
    }

    return ret;

device_failed:
    class_destroy(newChrdev.class);
class_failed:
    cdev_del(&newChrdev.cdev);
dev_add_filed:
    unregister_chrdev_region(newChrdev.devid, NEW_CHRDEV_BASE_COUNT);
devid_failed:
    return ret;
}


static void __exit chrdev_base_exit(void)
{
    /** 删除字符设备 **/
    cdev_del(&newChrdev.cdev);
    
    /** 注销设备号 **/
    unregister_chrdev_region(newChrdev.devid, NEW_CHRDEV_BASE_COUNT);

    /** 摧毁设备 **/
    device_destroy(newChrdev.class, newChrdev.devid);

    /** 摧毁类 **/
    class_destroy(newChrdev.class);
}


module_init(chrdev_base_init);
module_exit(chrdev_base_exit);
MODULE_LICENSE("GPL");
