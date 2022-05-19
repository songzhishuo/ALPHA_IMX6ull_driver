#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>

static int __init led_init(void);
static void __exit led_exit(void);
void led_reg_init(void);

// GPIO: GPIO1_IO03
#define DEV_NAME "char_led";

#define IMX6U_CCM_CCGR1 0x20C406C  // 20C_406C              0xFFFFFFFF
#define SW_MUX_GPIO1_IO3 0x20E0068 // 20E_0068              0x05
#define SW_PAD_GPIO1_IO3 0x20E02F4 // 20E_02F4              0x2A6009
#define GPIO1_GDIR 0x209c004       // 209_C004              1<<3
#define GPIO1_DR 0x209c000         // 209_C000              cmd

typedef struct
{
    uint8_t *dev_name; /*设备名*/
    uint8_t dev_cnt;   /*设备个数*/
    dev_t devno;       /*设备号*/

    struct cdev char_dev; /*字符设备*/

    void __iomem *remap_ccm_ccgr1; /*虚拟内存映射*/
    void __iomem *remap_sw_mux_gpio1_io3;
    void __iomem *remap_sw_pad_gpio1_io3;
    void __iomem *remap_gpio1_gdir;
    void __iomem *remap_gpio1_dr
} char_dev_hello_typedef;

char_dev_hello_typedef dev_info;

void led_ctl(uint8_t cmd)
{

    iowrite32(cmd << 3, dev_info.remap_gpio1_dr); //默认写高电平
}

void led_reg_init()
{

    dev_info.remap_ccm_ccgr1 = ioremap(IMX6U_CCM_CCGR1, 4);
    dev_info.remap_sw_mux_gpio1_io3 = ioremap(SW_MUX_GPIO1_IO3, 4);
    dev_info.remap_sw_pad_gpio1_io3 = ioremap(SW_PAD_GPIO1_IO3, 4);
    dev_info.remap_gpio1_gdir = ioremap(GPIO1_GDIR, 4);
    dev_info.remap_gpio1_dr = ioremap(GPIO1_DR, 4);

    /*step1: 使能GPIO时钟*/
    iowrite32(0xFFFFFFFF, dev_info.remap_ccm_ccgr1);
    /*step2: 配置GPIO 复用功能*/
    iowrite32(0x05, dev_info.remap_sw_mux_gpio1_io3);
    /*step3: 设置引脚属性(上下拉/速率/驱动能力)*/
    iowrite32(0x2A6009, dev_info.remap_sw_pad_gpio1_io3);
    /*step4: 控制GPIO 输出高低电平*/
    iowrite32(1 << 3, dev_info.remap_gpio1_gdir);
    //默认写高电平
    iowrite32(1 << 3, dev_info.remap_gpio1_dr);
}

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

    uint8_t data[32] = {0};
    uint8_t data_len = 32;
    if (count > data_len)
    {
        data_len = 32;
    }
    else
    {
        data_len = count;
    }

    /*从用户控件拷贝内存到内核*/
    if (copy_from_user(data, buffer, data_len))
    { //拷贝失败
        ret = -EINVAL;
        goto OUT;
    }

    if (data[0] == '1') // cmd
    {

        led_ctl(1);
        printk("close led\r\n");
    }
    else
    {
        led_ctl(0);
        printk("open led\r\n");
    }

    ret = data_len; //打开成功 一定要返回data_len 否则应用层会重复打开
OUT:
    return ret;
}

static int char_open(struct inode *inode, struct file *filp)
{
    ssize_t ret;
    printk("char open\r\n");

    return ret;
}

/**
 * @brief
 *
 * @param inode
 * @param file
 * @return int
 */
static int char_release(struct inode *inode, struct file *file)
{
    ssize_t ret = 0;
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
static int __init led_init(void)
{
    int ret = 0;
    printk("led module init\r\n");

    led_reg_init();

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
static void __exit led_exit(void)
{
    cdev_del(&dev_info.char_dev);
    unregister_chrdev_region(dev_info.devno, dev_info.dev_cnt);

    /*释放虚拟内存*/
    iounmap(dev_info.remap_ccm_ccgr1);
    iounmap(dev_info.remap_sw_mux_gpio1_io3);
    iounmap(dev_info.remap_sw_pad_gpio1_io3);
    iounmap(dev_info.remap_gpio1_gdir);
    iounmap(dev_info.remap_gpio1_dr);

    printk("led module exit");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("argon ");
