#include <linux/netdevice.h>
#include <linux/if.h>

#include "ifaces.h"

void
ifdata_init(struct ifdata * data, const char * name)
{
    memcpy(data->name, name, IFNAMSIZ);
    data->bw            = 0;
    data->bw_attr.name  = name;
    data->bw_attr.mode  = 0666;
    data->delay         = 0;
    data->delay_attr.name  = name;
    data->delay_attr.mode  = 0666;
    data->jitter        = 0;
    data->jitter_attr.name  = name;
    data->jitter_attr.mode  = 0666;
    data->price         = 0;
    data->price_attr.name  = name;
    data->price_attr.mode  = 0666;
}

void
ifaces_print(struct ifaces * ifaces)
{
    int i;
    struct ifdata * cur;
    for (i = 0; i<ifaces->count; i++) {
        cur = ifaces->data+i;
        printk(KERN_DEBUG "%s\n", cur->name);
    }
}

struct ifaces *
ifaces_create(struct net * net_ns)
{
    struct ifaces * ifaces;
    struct ifdata * ifdata;
    struct ifdata * new;
    struct net_device * net_dev;

    ifaces = kmalloc(sizeof(struct ifaces), GFP_KERNEL);
    if (!ifaces)
        return NULL;
    ifaces->count = 0;
    ifaces->data = NULL;

    for_each_netdev(net_ns, net_dev) {
        ifaces->count++;
        new = krealloc(ifaces->data,
                ifaces->count*sizeof(struct ifdata),
                GFP_KERNEL);
        if (!new) {
            ifaces->count--;
            ifaces_destroy(ifaces);
            return NULL;
        }
        ifaces->data = new;

        ifdata = ifaces->data+ifaces->count-1;
        ifdata_init(ifdata, net_dev->name);
    }

    return ifaces;
}

void
ifaces_destroy(struct ifaces * ifaces)
{
    kfree(ifaces->data);
    kfree(ifaces);
    return;
}

