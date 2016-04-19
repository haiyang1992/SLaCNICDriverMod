#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haiyang Han");
MODULE_DESCRIPTION("A module that intecepts a socket() system call and inits the laser in e1000 NIC");

statis int __init sock_intercept_init(void)
{
    printk(KERN_INFO "socket call intercept init\n");
    return 0;
}

static void __exit sock_intercept_exit(void)
{
    printk(KERN_INFO "socket call intercept exiting\n");
}

module_init(sock_intercept_init);
module_exit(sock_intercept_exit);
