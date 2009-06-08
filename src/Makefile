# Comment/uncomment to disable/enable debugging
DEBUG = y

ifeq ($(DEBUG),y)
    DEBFLAGS = -O -g -DIFACEPREF_DEBUG # -0 is needed to expand inlines
else
    DEBFLAGS = -O2
endif
EXTRA_CFLAGS += $(DEBFLAGS)

.PHONY: install default clean

install: default
	cp netqos.ko   /tmp/
	cp netqos_load /tmp/
	cp netqos_unload /tmp/
	cp netqos_test /tmp/
	cp netqos_watch /tmp/
	mkdir -p ${HOME}/man/man4/
	cp netqos.4  ${HOME}/man/man4/

# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
    netqos-objs := main.o ifaces.o 
    obj-m := netqos.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else

    extdatadir := $(shell cat ../extdata-generator/CONFIGUREME | egrep -v '^\#.*$$' | grep EXTDATADIR | cut -d'=' -f2)
    kernelversion := $(shell cat ../extdata-generator/CONFIGUREME | egrep -v '^\#.*$$' | grep KERNELVERSION | cut -d'=' -f2)
    modulesdir := $(extdatadir)/linux-src/linux-$(kernelversion)/modules/lib/modules/$(kernelversion)/build
    KERNELDIR ?= $(modulesdir)
    PWD := $(shell pwd)

default: netqos_test netqos_watch
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=um modules

netqos_test: test.o
	$(CC) -Wall -g -o netqos_test test.o

test.o: test.c
	$(CC) -Wall -g -c test.c

netqos_watch: watch.o
	$(CC) -Wall -g -o netqos_watch watch.o

watch.o: watch.c
	$(CC) -Wall -g -c watch.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=um clean
	- rm -f modules.order
	- rm -f *.o
	- rm -f netqos_test netqos_watch

endif

