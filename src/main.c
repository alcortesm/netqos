#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION("0.1");

static int __init netqos_init(void) {
    return 0; /* success */
}

static void __exit netqos_exit(void) {
    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
