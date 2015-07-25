/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * Parts of this driver are based on the following:
 *  - Kvaser linux leaf driver (version 4.78)
 *  - CAN driver for esd CAN-USB/2
 *
 * Copyright (C) 2002-2006 KVASER AB, Sweden. All rights reserved.
 * Copyright (C) 2010 Matthias Fuchs <matthias.fuchs@esd.eu>, esd gmbh
 * Copyright (C) 2012 Olivier Sobrie <olivier@sobrie.be>
 * Copyright (C) 2015 Thorsten Wilmer <wilmer.thorsten@gmail.com>
 */

#include <linux/completion.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/usb.h>

#include <linux/can.h>
#include <linux/can/dev.h>
#include <linux/can/error.h>

#define MAX_NET_DEVICES			2
#define CANTWI_VENDOR_ID		0x16C0
#define USB_TWITARGET_DEVEL_PRODUCT_ID 0x03e8
#define MAX_RX_URBS			4

static const struct usb_device_id cantwi_usb_table[] = {
	{ USB_DEVICE(CANTWI_VENDOR_ID, USB_TWITARGET_DEVEL_PRODUCT_ID) },
	
	{ }
};

struct cantwi_usb_net_priv {

};

struct cantwi_usb {
	struct usb_endpoint_descriptor *bulk_in, *bulk_out;
    struct usb_device *udev;
    struct usb_anchor rx_submitted;
    
    
    
	u32 fw_version;
	unsigned int nchannels;

	bool rxinitdone;
	void *rxbuf[MAX_RX_URBS];
	dma_addr_t rxbuf_dma[MAX_RX_URBS];
};


static int cantwi_usb_get_endpoints(const struct usb_interface *intf,
				    struct usb_endpoint_descriptor **in,
				    struct usb_endpoint_descriptor **out)
{
	const struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i;

	iface_desc = &intf->altsetting[0];

    // TODO use the correct endpoints
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (!*in && usb_endpoint_is_bulk_in(endpoint))
			*in = endpoint;

		if (!*out && usb_endpoint_is_bulk_out(endpoint))
			*out = endpoint;

		/* use first bulk endpoint for in and out */
		if (*in && *out)
			return 0;
	}

	return -ENODEV;
}

static int cantwi_usb_probe(struct usb_interface *intf,
			    const struct usb_device_id *id)
{
	struct cantwi_usb *dev;
	int err = -ENOMEM;
	int i;
	
	printk("My Debugger is Printk\n");

	dev = devm_kzalloc(&intf->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	err = cantwi_usb_get_endpoints(intf, &dev->bulk_in, &dev->bulk_out);
	if (err) {
		dev_err(&intf->dev, "Cannot get usb endpoint(s)");
		return err;
	}

	dev->udev = interface_to_usbdev(intf);

	init_usb_anchor(&dev->rx_submitted);

	usb_set_intfdata(intf, dev);

	

	dev_dbg(&intf->dev, "Firmware version: %d.%d.%d\n",
		((dev->fw_version >> 24) & 0xff),
		((dev->fw_version >> 16) & 0xff),
		(dev->fw_version & 0xffff));

	

	return 0;
}


static void cantwi_usb_remove_interfaces(struct cantwi_usb *dev)
{
	int i;

	/*for (i = 0; i < dev->nchannels; i++) {
		if (!dev->nets[i])
			continue;

		unregister_netdev(dev->nets[i]->netdev);
	}*/

	// kvaser_usb_unlink_all_urbs(dev);

	/*for (i = 0; i < dev->nchannels; i++) {
		if (!dev->nets[i])
			continue;

		free_candev(dev->nets[i]->netdev);
	}*/
}

static void cantwi_usb_disconnect(struct usb_interface *intf)
{
	struct cantwi_usb *dev = usb_get_intfdata(intf);

	usb_set_intfdata(intf, NULL);

	if (!dev)
		return;

	cantwi_usb_remove_interfaces(dev);
}


static struct usb_driver cantwi_usb_driver = {
	.name = "can_twi",
	.probe = cantwi_usb_probe,
	.disconnect = cantwi_usb_disconnect,
	.id_table = cantwi_usb_table,
};

module_usb_driver(cantwi_usb_driver);

MODULE_AUTHOR("Thorsten Wilmer <wilmer.thorsten@gmail.com>");
MODULE_DESCRIPTION("CAN driver for my LPC1769 based board/firmware");
MODULE_LICENSE("GPL v2");
