#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>


#define GPREAD_DEV_NAME "gpio_test"
#define GPREAD_DEV_COUNT 1

struct gp_dev_t{
    dev_t devid;        //设备号
    struct cdev cdev;   //cdev
    struct class *class;    //类
    struct device *device;  //设备
    unsigned int gpio;
    struct regulator *myregulator;
};


static struct gp_dev_t gpiodev; 

static int gpiodev_open(struct inode *pnode, struct file *pfile)
{
    
    return 0;
}

static ssize_t gpiodev_read(struct file *pfile, char __user *buffer, size_t count, loff_t *ppos)
{
    char *buff;
    if(gpio_get_value(gpiodev.gpio))
        buff = "high";
    else
        buff = "low";
    
    return copy_to_user(buffer, buff, strlen(buff));
}

static ssize_t gpiodev_write(struct file *pfile,const char __user *buffer, size_t count, loff_t *ppos)
{
    return 0;
}

static int gpiodev_release(struct inode *pnode, struct file *pfile)
{
    
    return 0;
}

static struct file_operations newChrOps = {
    .open = gpiodev_open,
    .release = gpiodev_release,
    .read = gpiodev_read,
    .write = gpiodev_write,
};

static int gpio_drv_probe(struct platform_device *pdev)
{
    int ret = 0;
    int major = 0;
    
    if(major){     /** 给定主设备号,注册设备号 **/
        gpiodev.devid = MKDEV(major, 0);
        ret = register_chrdev_region(gpiodev.devid, GPREAD_DEV_COUNT, GPREAD_DEV_NAME);
    } else {                /** 未给定主设备号,申请设备号 **/
        ret = alloc_chrdev_region(&gpiodev.devid, 0, GPREAD_DEV_COUNT, GPREAD_DEV_NAME);
    }
    if(ret < 0){
        printk("gpread chrdev_region error! \r\n");
        goto devid_failed;
    }

    /** 注册字符设备 **/
    gpiodev.cdev.owner = THIS_MODULE;
    cdev_init(&gpiodev.cdev, &newChrOps);
    ret = cdev_add(&gpiodev.cdev, gpiodev.devid, GPREAD_DEV_COUNT);
    if(ret < 0){
        printk("gpread add failed \r\n");
        goto dev_add_filed;
    }

    /** 自动创建设备节点 **/
    gpiodev.class = class_create(gpiodev.cdev.owner, GPREAD_DEV_NAME);     //创建类
    if(IS_ERR(gpiodev.class)){
        ret = PTR_ERR(gpiodev.class);
        goto class_failed;
    }

    gpiodev.device = device_create(gpiodev.class, NULL, gpiodev.devid, NULL, GPREAD_DEV_NAME);
    if(IS_ERR(gpiodev.device)){
        ret = PTR_ERR(gpiodev.device);
        goto device_failed;
    }
    if(!pdev->dev.of_node){
        ret = -EINVAL;
        dev_err(&pdev->dev, "invaild node \r\n");
        goto gpio_invailed;
    }
    gpiodev.gpio = of_get_named_gpio(pdev->dev.of_node, "rd-gpio", 0);
    if(!gpio_is_valid(gpiodev.gpio)){
        ret = -EINVAL;
        dev_err(&pdev->dev, "invaild rd_gpio property \r\n");
        goto gpio_invailed;
    }
    ret = devm_gpio_request(&pdev->dev, gpiodev.gpio, "gpio_read");
    if(ret){
        dev_err(&pdev->dev, "cannot request rd_gpio \r\n");
        goto gpio_invailed;
    }
    gpiodev.myregulator = regulator_get(&pdev->dev, "my_vcc");
    if(IS_ERR(gpiodev.myregulator)){
        dev_err(&pdev->dev, "regulator get failed \r\n");
        ret = PTR_ERR(gpiodev.myregulator);
        goto regulator_failed;
    }
    ret = regulator_enable(gpiodev.myregulator);
    if(ret){
        dev_err(&pdev->dev, "regulator enable failed \r\n");
        goto regulator_failed;
    }
    

    printk("my test gpio probe \r\n");
    return ret;


regulator_failed:
    devm_gpio_free(&pdev->dev, gpiodev.gpio);
gpio_invailed:
    device_destroy(gpiodev.class, gpiodev.devid);
device_failed:
    class_destroy(gpiodev.class);
class_failed:
    cdev_del(&gpiodev.cdev);
dev_add_filed:
    unregister_chrdev_region(gpiodev.devid, GPREAD_DEV_COUNT);
devid_failed:
    return ret;
}

static int gpio_drv_remove(struct platform_device *pdev){

    devm_gpio_free(&pdev->dev, gpiodev.gpio);
    /** 删除字符设备 **/
    cdev_del(&gpiodev.cdev);
    
    /** 注销设备号 **/
    unregister_chrdev_region(gpiodev.devid, GPREAD_DEV_COUNT);

    /** 摧毁设备 **/
    device_destroy(gpiodev.class, gpiodev.devid);

    /** 摧毁类 **/
    class_destroy(gpiodev.class);
    
    return 0;
}

static const struct of_device_id gpread_of_match[] = {
    { .compatible = "wangzl,gp_read_test", 0},
    { },
};

MODULE_DEVICE_TABLE(of, gpread_of_match);

static struct platform_driver gpioread_drv = {
    .probe = gpio_drv_probe,
    .remove = gpio_drv_remove,
    .driver = {
        .name = GPREAD_DEV_NAME,
        .of_match_table = of_match_ptr(gpread_of_match),
    },
};

module_platform_driver(gpioread_drv);

MODULE_LICENSE("GPL");
