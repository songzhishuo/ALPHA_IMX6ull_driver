#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init hello_init(void);
static void __exit hello_exit(void);

/**
 * @brief 驱动注册入口
 * 
 * @return int 驱动注册状态
 */
static int __init hello_init(void)
{
    printk(KERN_EMERG "[ KERN_EMERG ]  Hello  Module Init\n");
    printk( "[ default ]  Hello  Module Init\n");
    return 0;
}

/**
 * @brief 驱动卸载入口
 * 
 */
static void __exit hello_exit(void)
{
    printk("[ default ]   Hello  Module Exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("argon ");
