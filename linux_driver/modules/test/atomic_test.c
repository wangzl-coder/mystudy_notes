#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
//#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>

#define SLOCK_DEV_CNT 1
#define SLOCK_DEV_NAME "slock_dev"


struct slock_dev{
    dev_t devid;
    struct cdev dev;
    struct class *class;
    struct device *device;
    //unsigned int count;
    //spinlock_t spin_lock;
    atomic_t count;
};

static struct slock_dev slk_dev;

static int slock_dev_open(struct inode *pnode, struct file *pfile)
{
    atomic_set(&slk_dev.count, 0);
    return 0;
}

static int slock_dev_release(struct inode *pnode, struct file *pfile)
{

    return 0;
}

static ssize_t slock_dev_read(struct file *pfile, char __user *buff, size_t count, loff_t *ppos)
{
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    //spin_lock(&slk_dev.spin_lock);
    snprintf(buffer, sizeof(buffer), "%d", atomic_read(&slk_dev.count));
    //spin_unlock(&slk_dev.spin_lock);
    if(!copy_to_user(buff, buffer, count))
        return 0;
    else
        return -EINVAL;
}

static ssize_t slock_dev_write(struct file *pfile, const char __user *buff, size_t count, loff_t *ppos)
{
    char buffer[10];
    char *end;
    unsigned int val;
    memset(buffer, 0, sizeof(buffer));
    if(!copy_from_user(buffer, buff, 10)){
        val = (unsigned int) simple_strtoul(buffer, &end, 0);
        //spin_lock(&slk_dev.spin_lock);
        atomic_add(val, &slk_dev.count);
        //spin_unlock(&slk_dev.spin_lock);
        return 0;
    }
    return -EINVAL;
}


static const struct file_operations slock_ops = {
    .open = slock_dev_open,
    .release = slock_dev_release,
    .read = slock_dev_read,
    .write = slock_dev_write,
};


static int __init slock_init(void)
{
    int major = 0;
    int ret;

    if(major)
    {
        slk_dev.devid = MKDEV(major, 0);
        ret = register_chrdev_region(slk_dev.devid, SLOCK_DEV_CNT, SLOCK_DEV_NAME);
    } else {
        ret = alloc_chrdev_region(&slk_dev.devid, 0, SLOCK_DEV_CNT, SLOCK_DEV_NAME);
    }
    if(ret < 0){
        printk("devid regiset failed \r\n");
        goto failed_devid;
    }

    cdev_init(&slk_dev.dev, &slock_ops);

    ret = cdev_add(&slk_dev.dev, slk_dev.devid, SLOCK_DEV_CNT);
    if(ret < 0){
        printk("register slock cdev failed \r\n");
        goto failed_cdev;
    }

    slk_dev.class =  class_create(THIS_MODULE, SLOCK_DEV_NAME);
    if(IS_ERR(slk_dev.class)){
        ret = PTR_ERR(slk_dev.class);
        printk("slock class create failed \r\n");
        goto failed_class;
    }
    slk_dev.device = device_create(slk_dev.class, NULL, slk_dev.devid, NULL, SLOCK_DEV_NAME);
    if(IS_ERR(slk_dev.device)){
        ret = PTR_ERR(slk_dev.device);
        printk("slock device create failed \r\n");
        goto failed_device;
    }
    //spin_lock_init(&slk_dev.spin_lock);
    return 0;


failed_device:
    class_destroy(slk_dev.class);
failed_class:
    cdev_del(&slk_dev.dev);
failed_cdev:
    unregister_chrdev_region(slk_dev.devid, SLOCK_DEV_CNT);
failed_devid:
    return ret;
}


static void slock_exit(void)
{
    device_destroy(slk_dev.class, slk_dev.devid);
    class_destroy(slk_dev.class);
    cdev_del(&slk_dev.dev);
    unregister_chrdev_region(slk_dev.devid, SLOCK_DEV_CNT);
    
}


module_init(slock_init);
module_exit(slock_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangzl");