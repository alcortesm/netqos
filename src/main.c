#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION("0.1");

static int __init netqos_init(void) {
    printk(KERN_DEBUG "netqos_init(): called\n");
    printk(KERN_DEBUG "netqos_init(): return 0 (success)\n");
    return 0; /* success */
}

static void __exit netqos_exit(void) {
    printk(KERN_DEBUG "netqos_exit(): called\n");
    printk(KERN_DEBUG "netqos_exit(): returns\n");
    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
