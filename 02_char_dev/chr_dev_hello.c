#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static int __init hello_init(void);
static void __exit hello_exit(void);

#define DEV_NAME "test_char_dev";

typedef struct
{
    uint8_t *dev_name; /*设备名*/
    uint8_t dev_cnt;   /*设备个数*/
    dev_t devno;       /*设备号*/

    struct cdev char_dev; /*字符设备*/

} char_dev_hello_typedef;

char_dev_hello_typedef dev_info;

static ssize_t char_read(struct file *fp, char __user *buf, size_t count, loff_t *ppos)
{
    ssize_t ret;
    printk("char read\r\n");

    return ret;
}

static ssize_t char_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    ssize_t ret;
    printk("char write\r\n");

    return ret;
}

static int char_open(struct inode *inode, struct file *filp)
{
    ssize_t ret;
    printk("char open\r\n");

    return ret;
}

static int char_release(struct inode *inode, struct file *file)
{
    ssize_t ret;
    printk("char release\r\n");

    return ret;
}

static const struct file_operations char_dev_fops = {
    .owner = THIS_MODULE,
    .open = char_open,
    .release = char_release,
    .read = char_read,
    .write = char_write,

};

/**
 * @brief 驱动注册入口
 *
 * @return int 驱动注册状态
 */
static int __init hello_init(void)
{
    int ret = 0;
    printk("hello module init\r\n");

    dev_info.dev_name = DEV_NAME;
    dev_info.dev_cnt = 1;

    // step1: 动态获取设备号
    ret = alloc_chrdev_region(&dev_info.devno, 0, dev_info.dev_cnt, dev_info.dev_name);
    if (ret < 0)
    {
        printk("fail to alloc dev num\r\n");
        goto alloc_err;
    }

    // step2: 关联cdev和设备号
    cdev_init(&dev_info.char_dev, &char_dev_fops);

    ret = cdev_add(&dev_info.char_dev, dev_info.devno, dev_info.dev_cnt);
    if (ret < 0)
    {
        printk("fail to add cdev\r\n");
        goto cdev_err;
    }

    // int cdev_add(struct cdev *, dev_t, unsigned);

    return 0;

cdev_err:
    //注销设备号
    unregister_chrdev_region(dev_info.devno, dev_info.dev_cnt);

alloc_err:
    return ret;
}

/**
 * @brief 驱动卸载入口
 *
 */
static void __exit hello_exit(void)
{
    printk("hello module exit");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("argon ");
