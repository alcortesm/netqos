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


/*
 * show functions for read only fix string attributes
 */
#define __NETQOS_ATTR_SHOW(func_name, str_name) \
    static ssize_t \
    func_name(struct kobject *kobj, \
        struct kobj_attribute *attr, char *buf) \
    {   \
        return snprintf(buf, PAGE_SIZE, "%s\n", str_name);    \
    }
__NETQOS_ATTR_SHOW(version_show, NETQOS_VERSION);
__NETQOS_ATTR_SHOW(bw_units_show, NETQOS_BW_UNITS);
__NETQOS_ATTR_SHOW(delay_units_show, NETQOS_DELAY_UNITS);
__NETQOS_ATTR_SHOW(jitter_units_show, NETQOS_JITTER_UNITS);
__NETQOS_ATTR_SHOW(price_units_show, NETQOS_PRICE_UNITS);



/*
 * attributes
 */
static struct kobj_attribute version_kobj_attr =
    __ATTR_RO(version);

/* __ATTR_RO for functions with different name than
 * the attribute name */
#define __NETQOS_ATTR_RO(_file_name, _func_name) { \
        .attr   = { .name = __stringify(_file_name), .mode = 0444 }, \
                .show   = _func_name##_show,                 \
}
static struct kobj_attribute bw_units_kobj_attr =
    __NETQOS_ATTR_RO(units, bw_units);
static struct kobj_attribute delay_units_kobj_attr =
    __NETQOS_ATTR_RO(units, delay_units);
static struct kobj_attribute jitter_units_kobj_attr =
    __NETQOS_ATTR_RO(units, jitter_units);
static struct kobj_attribute price_units_kobj_attr =
    __NETQOS_ATTR_RO(units, price_units);




/*
 * dirs
 */
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
    int err = 0;

    /* Create a simple "netqos" dir under /sys/kernel */
    netqos_kobj = kobject_create_and_add("netqos", kernel_kobj);
    if (!netqos_kobj) {
        err = -ENOMEM;
        goto err_out;
    }

    /* Populate the directory with the attributes */
    err = sysfs_create_file(netqos_kobj, &version_kobj_attr.attr);
    if (err)
        goto err_version;

    /* create subdirs */
    figures_kobj = kobject_create_and_add("figures", netqos_kobj);
    if (!figures_kobj) {
        err = -ENOMEM;
        goto err_figures;
    }
    bw_kobj = kobject_create_and_add("bw", figures_kobj);
    if (!bw_kobj) {
        err = -ENOMEM;
        goto err_bw;
    }
    delay_kobj = kobject_create_and_add("delay", figures_kobj);
    if (!delay_kobj) {
        err = -ENOMEM;
        goto err_delay;
    }
    jitter_kobj = kobject_create_and_add("jitter", figures_kobj);
    if (!jitter_kobj) {
        err = -ENOMEM;
        goto err_jitter;
    }
    price_kobj = kobject_create_and_add("price", figures_kobj);
    if (!price_kobj) {
        err = -ENOMEM;
        goto err_price;
    }
    ifaces_kobj = kobject_create_and_add("ifaces", netqos_kobj);
    if (!ifaces_kobj) {
        err = -ENOMEM;
        goto err_ifaces;
    }

    /* populate with units files */
    err = sysfs_create_file(bw_kobj, &bw_units_kobj_attr.attr);
    if (err)
        goto err_bw_units;
    err = sysfs_create_file(delay_kobj, &delay_units_kobj_attr.attr);
    if (err)
        goto err_delay_units;
    err = sysfs_create_file(jitter_kobj, &jitter_units_kobj_attr.attr);
    if (err)
        goto err_jitter_units;
    err = sysfs_create_file(price_kobj, &price_units_kobj_attr.attr);
    if (err)
        goto err_price_units;

    /* ifaces files */


    return 0;

err_price_units:
    sysfs_remove_file(jitter_kobj, &jitter_units_kobj_attr.attr);
err_jitter_units:
    sysfs_remove_file(delay_kobj, &delay_units_kobj_attr.attr);
err_delay_units:
    sysfs_remove_file(bw_kobj, &bw_units_kobj_attr.attr);
err_bw_units:
    kobject_put(ifaces_kobj);
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
err_out:
    return err;
}

static int __init
netqos_init(void)
{
    printk(KERN_INFO "netqos init (version %s)\n",
            NETQOS_VERSION);

    ifaces = netqos_ifaces_create(&init_net);

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

    destroy_init_ifaces(ifaces);

    sysfs_destroy_tree();

    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
