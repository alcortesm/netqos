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
/* show() and store() act as demultiplexers for other
 * functions and use attr and kobj as the selectors */
static ssize_t show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf);
static ssize_t store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf,
        size_t count);

/* attributes */
static struct kobj_attribute foo_attribute =
    __ATTR(foo, 0666, show, store);
static struct kobj_attribute bar_attribute =
    __ATTR(bar, 0666, show, store);
static struct attribute *attrs[] = {
    &foo_attribute.attr,
    &bar_attribute.attr,
    NULL,
};

/* attributes' storage */
static int foo;
static int bar;

static ssize_t
show_int(char *buf, int i)
{
    return sprintf(buf, "%d\n", i);
}

static ssize_t
store_int(const char *buf, size_t count, int *ip)
{
    sscanf(buf, "%du", ip);
    return count;
}

/* definitions of attribute accesss functions */
static ssize_t
show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    if (attr == &foo_attribute) {
        return show_int(buf, foo);
    }
    else if (attr == &bar_attribute) {
        return show_int(buf, bar);
    } else {
        return -1;
    }
}

static ssize_t
store(struct kobject *kobj, struct kobj_attribute * attr,
        const char *buf, size_t count)
{
    if (attr == &foo_attribute) {
        return store_int(buf, count, &foo);
    } else if (attr == &bar_attribute) {
        return store_int(buf, count, &bar);
    } else {
        return -1;
    }
}

/* Unnamed attribute group; will put all attributes
 * directly under the kobject dir instead of creating
 * a subdir.
 */
static struct attribute_group attr_group = {
    .attrs = attrs,
};

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
    r = sysfs_create_group(netqos_kobj, &attr_group);
    if (r)
        goto err_netqos_attr;

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
    sysfs_remove_group(netqos_kobj, &attr_group);
err_netqos_attr:
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
    sysfs_remove_group(netqos_kobj, &attr_group);
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
