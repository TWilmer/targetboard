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
#define MAX_TX_URBS			20
#define RX_BUFFER_SIZE			64


static const struct usb_device_id cantwi_usb_table[] = {
		{ USB_DEVICE(CANTWI_VENDOR_ID, USB_TWITARGET_DEVEL_PRODUCT_ID) },

		{ }
};

#define CANTWI_DEV_EXTID			0x01
#define CANTWI_DEV_RTR			    0x02

// TODO do we need this?
#define CANTWI_DEV_DATA_START		0x55
#define CANTWI_DEV_DATA_END	     	0xAA
/* tx frame */
struct __packed cantwi_dev_tx_msg {
	u8 begin;
	u8 flags;	/* RTR and EXT_ID flag */
	__be32 id;	/* upper 3 bits not used */
	u8 dlc;		/* data length code 0-8 bytes */
	u8 data[8];	/* 64-bit data */
	u8 end;
};


struct cantwi_dev_tx_urb_context {
	struct cantwi_usb_net_priv *priv;

	u32 echo_index;
	u8 dlc;
};

struct cantwi_usb_net_priv {
	struct can_priv can; /* must be the first member */
	struct usb_device *udev;
	struct net_device *netdev;
	struct usb_endpoint_descriptor *bulk_in, *bulk_out;
	struct usb_anchor rx_submitted;

	atomic_t active_tx_urbs;
	struct usb_anchor tx_submitted;
	struct cantwi_dev_tx_urb_context tx_contexts[MAX_TX_URBS];
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
/* Callback for reading data from device
 *
 * Check urb status, call read function and resubmit urb read operation.
 */
static void cantwi_bulk_callback(struct urb *urb)
{
// TODO fill it
}

/* Callback handler for write operations
 *
 * Free allocated buffers, check transmit status and
 * calculate statistic.
 */
static void usb_8dev_write_bulk_callback(struct urb *urb)
{
	struct cantwi_dev_tx_urb_context *context = urb->context;
	struct cantwi_usb_net_priv *priv;
	struct net_device *netdev;

	BUG_ON(!context);

	priv = context->priv;
	netdev = priv->netdev;

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);

	atomic_dec(&priv->active_tx_urbs);

	if (!netif_device_present(netdev))
		return;

	if (urb->status)
		netdev_info(netdev, "Tx URB aborted (%d)\n",
			 urb->status);

	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += context->dlc;

	can_get_echo_skb(netdev, context->echo_index);

	can_led_event(netdev, CAN_LED_EVENT_TX);

	/* Release context */
	context->echo_index = MAX_TX_URBS;

	netif_wake_queue(netdev);
}

/* Send open command to device */
static int cantwi_cmd_open(struct cantwi_usb_net_priv *priv)
{
	return 0;
}
static int cantwi_cmd_close(struct cantwi_usb_net_priv *priv)
{
	return 0;
}

// -----------------------------------------------------------
// Network Part
// -----------------------------------------------------------

/* Send data to device */
static netdev_tx_t cantwi_dev_start_xmit(struct sk_buff *skb,
				      struct net_device *netdev)
{
	struct cantwi_dev_tx_msg *msg;
	struct cantwi_usb_net_priv *priv = netdev_priv(netdev);
	struct net_device_stats *stats = &netdev->stats;
	struct can_frame *cf = (struct can_frame *) skb->data;

	struct urb *urb;
	struct cantwi_dev_tx_urb_context *context = NULL;
	u8 *buf;
	int i, err;
	size_t size = 10;

	if (can_dropped_invalid_skb(netdev, skb))
		return NETDEV_TX_OK;

	/* create a URB, and a buffer for it, and copy the data to the URB */
	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb) {
		netdev_err(netdev, "No memory left for URBs\n");
		goto nomem;
	}

	buf = usb_alloc_coherent(priv->udev, size, GFP_ATOMIC,
				 &urb->transfer_dma);
	if (!buf) {
		netdev_err(netdev, "No memory left for USB buffer\n");
		goto nomembuf;
	}

	memset(buf, 0, size);

	msg = (struct usb_8dev_tx_msg *)buf;
		msg->begin = CANTWI_DEV_DATA_START;
		msg->flags = 0x00;

		if (cf->can_id & CAN_RTR_FLAG)
			msg->flags |= CANTWI_DEV_RTR;

		if (cf->can_id & CAN_EFF_FLAG)
			msg->flags |= CANTWI_DEV_EXTID;

		msg->id = cpu_to_be32(cf->can_id & CAN_ERR_MASK);
		msg->dlc = cf->can_dlc;
		memcpy(msg->data, cf->data, cf->can_dlc);
		msg->end = CANTWI_DEV_DATA_END;

		for (i = 0; i < MAX_TX_URBS; i++) {
			if (priv->tx_contexts[i].echo_index == MAX_TX_URBS) {
				context = &priv->tx_contexts[i];
				break;
			}
		}

	/* May never happen! When this happens we'd more URBs in flight as
	 * allowed (MAX_TX_URBS).
	 */
	if (!context)
		goto nofreecontext;

	context->priv = priv;
	context->echo_index = i;
	context->dlc = cf->can_dlc;

	usb_fill_bulk_urb(urb, priv->udev,
			  usb_sndbulkpipe(priv->udev,priv->bulk_out->bEndpointAddress),
			  buf, size, usb_8dev_write_bulk_callback, context);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &priv->tx_submitted);

	can_put_echo_skb(skb, netdev, context->echo_index);

	atomic_inc(&priv->active_tx_urbs);

	err = usb_submit_urb(urb, GFP_ATOMIC);
	if (unlikely(err))
		goto failed;
	else if (atomic_read(&priv->active_tx_urbs) >= MAX_TX_URBS)
		/* Slow down tx path */
		netif_stop_queue(netdev);

	/* Release our reference to this URB, the USB core will eventually free
	 * it entirely.
	 */
	usb_free_urb(urb);

	return NETDEV_TX_OK;

nofreecontext:
	usb_free_coherent(priv->udev, size, buf, urb->transfer_dma);
	usb_free_urb(urb);

	netdev_warn(netdev, "couldn't find free context");

	return NETDEV_TX_BUSY;

failed:
	can_free_echo_skb(netdev, context->echo_index);

	usb_unanchor_urb(urb);
	usb_free_coherent(priv->udev, size, buf, urb->transfer_dma);

	atomic_dec(&priv->active_tx_urbs);

	if (err == -ENODEV)
		netif_device_detach(netdev);
	else
		netdev_warn(netdev, "failed tx_urb %d\n", err);

nomembuf:
	usb_free_urb(urb);

nomem:
	dev_kfree_skb(skb);
	stats->tx_dropped++;

	return NETDEV_TX_OK;
}

static void unlink_all_urbs(struct cantwi_usb_net_priv *priv)
{
	int i;

	usb_kill_anchored_urbs(&priv->rx_submitted);

	usb_kill_anchored_urbs(&priv->tx_submitted);
	atomic_set(&priv->active_tx_urbs, 0);

	for (i = 0; i < MAX_TX_URBS; i++)
		priv->tx_contexts[i].echo_index = MAX_TX_URBS;
}

/* Close USB device */
static int cantwi_dev_close(struct net_device *netdev)
{
	struct cantwi_usb_net_priv *priv = netdev_priv(netdev);
	int err = 0;

	/* Send CLOSE command to CAN controller */
	err = cantwi_cmd_close(priv);
	if (err)
		netdev_warn(netdev, "couldn't stop device");

	priv->can.state = CAN_STATE_STOPPED;

	netif_stop_queue(netdev);

	/* Stop polling */
	unlink_all_urbs(priv);

	close_candev(netdev);

	can_led_event(netdev, CAN_LED_EVENT_STOP);

	return err;
}


/* Start USB device */
static int cantwi_start(struct cantwi_usb_net_priv *priv)
{
	struct net_device *netdev = priv->netdev;
	int err, i;

	for (i = 0; i < MAX_RX_URBS; i++) {
		struct urb *urb = NULL;
		u8 *buf;

		/* create a URB, and a buffer for it */
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!urb) {
			netdev_err(netdev, "No memory left for URBs\n");
			err = -ENOMEM;
			break;
		}

		buf = usb_alloc_coherent(priv->udev, RX_BUFFER_SIZE, GFP_KERNEL,
					 &urb->transfer_dma);
		if (!buf) {
			netdev_err(netdev, "No memory left for USB buffer\n");
			usb_free_urb(urb);
			err = -ENOMEM;
			break;
		}


		usb_fill_bulk_urb(urb, priv->udev,
				  usb_rcvbulkpipe(priv->udev,
						  priv->bulk_in->bEndpointAddress),
				  buf, RX_BUFFER_SIZE,
				  cantwi_bulk_callback, priv);
		urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		usb_anchor_urb(urb, &priv->rx_submitted);

		err = usb_submit_urb(urb, GFP_KERNEL);
		if (err) {
			usb_unanchor_urb(urb);
			usb_free_coherent(priv->udev, RX_BUFFER_SIZE, buf,
					  urb->transfer_dma);
			usb_free_urb(urb);
			break;
		}

		/* Drop reference, USB core will take care of freeing it */
		usb_free_urb(urb);
	}

	/* Did we submit any URBs */
	if (i == 0) {
		netdev_warn(netdev, "couldn't setup read URBs\n");
		return err;
	}

	/* Warn if we've couldn't transmit all the URBs */
	if (i < MAX_RX_URBS)
		netdev_warn(netdev, "rx performance may be slow\n");

	err = cantwi_cmd_open(priv);
	if (err)
		goto failed;

	priv->can.state = CAN_STATE_ERROR_ACTIVE;

	return 0;

failed:
	if (err == -ENODEV)
		netif_device_detach(priv->netdev);

	netdev_warn(netdev, "couldn't submit control: %d\n", err);

	return err;
}

/* Open USB device */
static int cantwi_dev_open(struct net_device *netdev)
{
	struct cantwi_usb_net_priv *priv = netdev_priv(netdev);
	int err;

	/* common open */
	err = open_candev(netdev);
	if (err)
		return err;

	can_led_event(netdev, CAN_LED_EVENT_OPEN);

	/* finally start device */
	err = cantwi_start(priv);
	if (err) {
		if (err == -ENODEV)
			netif_device_detach(priv->netdev);

		netdev_warn(netdev, "couldn't start device: %d\n",
			 err);

		close_candev(netdev);

		return err;
	}

	netif_start_queue(netdev);

	return 0;
}
static const struct net_device_ops cantwi_netdev_ops = {
	.ndo_open = cantwi_dev_open,
	.ndo_stop = cantwi_dev_close,
	.ndo_start_xmit = cantwi_dev_start_xmit,
	.ndo_change_mtu = can_change_mtu,
};


// -----------------------------------------------------------
// USB Part
// -----------------------------------------------------------

static int cantwi_usb_get_endpoints(const struct usb_interface *intf,
		struct usb_endpoint_descriptor **in,
		struct usb_endpoint_descriptor **out)
{
	const struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i;


	iface_desc = &intf->altsetting[0];

	if( (iface_desc->desc.bInterfaceSubClass !=0xFA)
			&&(iface_desc->desc.bInterfaceSubClass !=0xFB))
	{
		*out =0;
		*in =0;
		return -ENODEV;
	}

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
	struct net_device *netdev;
	struct cantwi_usb *dev;
	struct cantwi_usb_net_priv *priv;
	int err = -ENOMEM;
	struct usb_device *usbdev = interface_to_usbdev(intf);


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

	netdev = alloc_candev(sizeof(struct cantwi_usb_net_priv), MAX_TX_URBS);
	if (!netdev) {
		dev_err(&intf->dev, "Couldn't alloc candev\n");
		return -ENOMEM;
	}

	priv = netdev_priv(netdev);

	priv->udev = usbdev;
	priv->netdev = netdev;
	priv->bulk_in=dev->bulk_in;
	priv->bulk_out=dev->bulk_out;

	dev_dbg(&intf->dev, "Firmware version: %d.%d.%d\n",
			((dev->fw_version >> 24) & 0xff),
			((dev->fw_version >> 16) & 0xff),
			(dev->fw_version & 0xffff));



	return 0;
}


static void cantwi_usb_remove_interfaces(struct cantwi_usb *dev)
{


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
