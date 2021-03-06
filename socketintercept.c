#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/netdevice.h>
//#include <sys/socket.h>
//#include <sys/types.h>
#include "e1000.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haiyang Han");
MODULE_DESCRIPTION("A module that intecepts a socket() system call and inits the laser in e1000 NIC");
MODULE_VERSION("0.1");

unsigned long *sys_call_table;

extern void e1000_laser_init(struct net_device *netdev);
extern void e1000_laser_sock_close(struct net_device *netdev);
//extern int getsockname(int sockfd, struct sockaddr *addr, int *addrlen);

asmlinkage int (*original_sendmsg) (int sockfd, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags);
asmlinkage int e1000_sendmsg(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags)
{
    struct net_device *dev;
    printk(KERN_INFO "socket() intercepted\nNow turning on laser\n"); 
    dev = first_net_device(&init_net);
    // assume current dev is eth0, using e1000
    printk(KERN_INFO"@ [%s]\n", dev->name);
    if (dev->name == "eth0") {
        printk(KERN_INFO "Now turning on laser\n");
        //e1000_laser_init(dev);
    }
    else {
        printk(KERN_INFO "NIC not e1000!\n");
    }
    
    return original_sendmsg(sockfd, msgvec, vlen, flags);
};

/*asmlinkage long (*original_close) (unsigned int fd);*/
/*asmlinkage long e1000_close(unsigned int fd)*/
/*{ */
    /*struct sockaddr addr;*/
    /*int addrlen = sizeof(addr);*/
    /*printk(KERN_INFO "close() intercepted\nNow turning off laser\n");*/
    /*//if (getsockname(fd, &addr, &addrlen) == -1)*/
    /*if (getsockname(fd, &addr, &addrlen) == -1)*/
    /*{*/
        /*printk(KERN_INFO"not a socket close\n");*/
        /*return original_close(fd);*/
    /*}*/
    /*else*/
    /*{*/
        /*struct net_device *dev;*/
        /*dev = first_net_device(&init_net);*/
        /*// assume current dev is eth0, using e1000*/
        /*printk(KERN_INFO"@ [%s]\n", dev->name);*/
        /*if (dev->name == "eth0") {*/
            /*printk(KERN_INFO "Running laser sock close\n");*/
            /*e1000_laser_sock_close(dev);*/
        /*}*/
        /*else {*/
            /*printk(KERN_INFO "NIC not e1000!\n");*/
        /*}*/
    
        /*return original_close(fd);*/
     /*}*/
/*};*/

static int __init socketintercept_init(void)
{
    printk(KERN_INFO "socket call intercept init\n");
    /* obtain sys_call_table from hardcoded value found in System.map */
    *(long *)&sys_call_table = 0xffffffff81a001c0;

    /* store original location of socket(). Alter sys_call_table to point to our functions*/
    original_sendmsg = (void *)xchg(&sys_call_table[__NR_sendmsg], e1000_sendmsg);
//    original_close = (void *)xchg(&sys_call_table[__NR_close], e1000_close);
    return 0;
}

static void __exit socketintercept_exit(void)
{
    /* restore original socket() in sys_call_table */
    xchg(&sys_call_table[__NR_sendmsg], original_sendmsg);
  //  xchg(&sys_call_table[__NR_close], original_close);

    printk(KERN_INFO "socket call intercept exiting\n");
}

module_init(socketintercept_init);
module_exit(socketintercept_exit);
