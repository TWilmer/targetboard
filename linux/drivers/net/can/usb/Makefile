#
#  $Id$
#

ifeq ($(KERNELRELEASE),)

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)
TOPDIR    := $(PWD)/../..

modules modules_install clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@ TOPDIR=$(TOPDIR)

export CONFIG_TWICAN=m

else

-include $(TOPDIR)/Makefile.common

obj-$(CONFIG_TWICAN)	+= can-twi.o
twi-objs		:= twi.o 


endif
