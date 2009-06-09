#ifndef _IFACES_H_
#define _IFACES_H_

#include <linux/netdevice.h>

struct ifdata {
    struct ifaces * ifaces;
    char name[IFNAMSIZ];
    int bw;
    struct kobj_attribute bw_kobj_attr;
    int delay;
    struct attribute delay_attr;
    int jitter;
    struct attribute jitter_attr;
    int price;
    struct attribute price_attr;
};

void ifdata_init (struct ifdata * data, const char * name);


struct ifaces {
    int count;
    struct ifdata * data; /* array */
};

void ifaces_print (struct ifaces * ifaces);
struct ifaces * ifaces_create (struct net * net_ns);
void  ifaces_destroy (struct ifaces * ifaces);

#endif /* _IFACES_H_ */
