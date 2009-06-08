#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "netqos.h"
#include "ifaces.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION(NETQOS_VERSION);

static ssize_t
version_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    struct netqos_ifaces * ifaces;
    
    ifaces = netqos_ifaces_create(&init_net);
    netqos_ifaces_print(ifaces);
    netqos_ifaces_destroy(ifaces);

    return snprintf(buf, PAGE_SIZE, "%s\n", NETQOS_VERSION);
}

/* attributes */
static struct kobj_attribute version_kobj_attr =
    __ATTR_RO(version);

static struct kobject *netqos_kobj;
static struct kobject *figures_kobj;
static struct kobject *ifaces_kobj;
static struct kobject *bw_kobj;
static struct kobject *delay_kobj;
static struct kobject *jitter_kobj;
static struct kobject *price_kobj;

static int __init
sysfs_build_tree(void)
{
    int r;

    /* Create a simple "netqos" dir under /sys/kernel */
    netqos_kobj = kobject_create_and_add("netqos", kernel_kobj);
    if (!netqos_kobj) {
        r = -ENOMEM;
        goto err;
    }

    /* Populate the directory with the attributes */
    r = sysfs_create_file(netqos_kobj, &version_kobj_attr.attr);
    if (r)
        goto err_version;

    /* create subdirs */
    figures_kobj = kobject_create_and_add("figures", netqos_kobj);
    if (!figures_kobj) {
        r = -ENOMEM;
        goto err_figures;
    }
    bw_kobj = kobject_create_and_add("bw", figures_kobj);
    if (!bw_kobj) {
        r = -ENOMEM;
        goto err_bw;
    }
    delay_kobj = kobject_create_and_add("delay", figures_kobj);
    if (!delay_kobj) {
        r = -ENOMEM;
        goto err_delay;
    }
    jitter_kobj = kobject_create_and_add("jitter", figures_kobj);
    if (!jitter_kobj) {
        r = -ENOMEM;
        goto err_jitter;
    }
    price_kobj = kobject_create_and_add("price", figures_kobj);
    if (!price_kobj) {
        r = -ENOMEM;
        goto err_price;
    }
    ifaces_kobj = kobject_create_and_add("ifaces", netqos_kobj);
    if (!ifaces_kobj) {
        r = -ENOMEM;
        goto err_ifaces;
    }

    return 0;

err_ifaces:
    kobject_put(price_kobj);
err_price:
    kobject_put(jitter_kobj);
err_jitter:
    kobject_put(delay_kobj);
err_delay:
    kobject_put(bw_kobj);
err_bw:
    kobject_put(figures_kobj);
err_figures:
    sysfs_remove_file(netqos_kobj, &version_kobj_attr.attr);
err_version:
    kobject_put(netqos_kobj);
err:
    return r;
}

static int __init
netqos_init(void)
{
    printk(KERN_INFO "netqos init (version %s)\n",
            NETQOS_VERSION);

    return sysfs_build_tree();
}

static void __exit
sysfs_destroy_tree(void)
{
    kobject_put(price_kobj);
    kobject_put(jitter_kobj);
    kobject_put(delay_kobj);
    kobject_put(bw_kobj);
    kobject_put(figures_kobj);
    kobject_put(ifaces_kobj);
    sysfs_remove_file(netqos_kobj, &version_kobj_attr.attr);
    kobject_put(netqos_kobj);
}

static void __exit
netqos_exit(void)
{
    printk(KERN_DEBUG "netqos exit\n");

    sysfs_destroy_tree();
    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
