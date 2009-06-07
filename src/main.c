#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "netqos.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION(NETQOS_VERSION);

/* declaration of attribute access functions */
static ssize_t version_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf);

/* attributes */
static struct kobj_attribute version_kobj_attr =
    __ATTR_RO(version);

static ssize_t
version_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%s\n", NETQOS_VERSION);
}

static struct kobject *netqos_kobj;
static struct kobject *figures_kobj;
static struct kobject *ifaces_kobj;
static int __init
sysfs_build_tree(void)
{
    int r;
    /* Create a simple kobject called "netqos" under /sys/kernel
     * 
     * As this is a simple directory, no uevent will be sent to
     * userspace.
     */
    netqos_kobj = kobject_create_and_add("netqos", kernel_kobj);
    if (!netqos_kobj) {
        r = -ENOMEM;
        goto err;
    }

    /* Populate the directory with the attributes */
    r = sysfs_create_file(netqos_kobj, &version_kobj_attr.attr);
    if (r)
        goto err_version;

    /* create "figures/" and "ifaces/" subdirs */
    figures_kobj = kobject_create_and_add("figures", netqos_kobj);
    if (!figures_kobj) {
        r = -ENOMEM;
        goto err_figures;
    }
    ifaces_kobj = kobject_create_and_add("ifaces", netqos_kobj);
    if (!ifaces_kobj) {
        r = -ENOMEM;
        goto err_ifaces;
    }

    return 0;

err_ifaces:
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
    printk(KERN_INFO "netqos init (API version %s)\n",
            NETQOS_VERSION);

    return sysfs_build_tree();
}

static void __exit
sysfs_destroy_tree(void)
{
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
