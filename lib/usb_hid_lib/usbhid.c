#include "usbhid.h"

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,		/* USB 2.0 */
	.bDeviceClass = USB_CLASS_HID,
	.bDeviceSubClass = USB_HID_SUBCLASS_BOOT_INTERFACE,
	.bDeviceProtocol = USB_HID_INTERFACE_PROTOCOL_MOUSE,
	.bMaxPacketSize0 = USB_HID_PACKET_SIZE,
	.idVendor = 0x0483,		/* STM */
	.idProduct = 0x5710,	/* Joystick in FS Mode */
	.bcdDevice = 0x0100,	/* Device Release Number: 1.0 */
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const uint8_t hid_report_descriptor[] = {
	0x05, 0x01, /* USAGE_PAGE (Generic Desktop)         */
	0x09, 0x02, /* USAGE (Mouse)                        */
	0xa1, 0x01, /* COLLECTION (Application)             */
	0x09, 0x01, /*   USAGE (Pointer)                    */
	0xa1, 0x00, /*   COLLECTION (Physical)              */
	0x05, 0x09, /*     USAGE_PAGE (Button)              */
	0x19, 0x01, /*     USAGE_MINIMUM (Button 1)         */
	0x29, 0x03, /*     USAGE_MAXIMUM (Button 3)         */
	0x15, 0x00, /*     LOGICAL_MINIMUM (0)              */
	0x25, 0x01, /*     LOGICAL_MAXIMUM (1)              */
	0x95, 0x03, /*     REPORT_COUNT (3)                 */
	0x75, 0x01, /*     REPORT_SIZE (1)                  */
	0x81, 0x02, /*     INPUT (Data,Var,Abs)             */
	0x95, 0x01, /*     REPORT_COUNT (1)                 */
	0x75, 0x05, /*     REPORT_SIZE (5)                  */
	0x81, 0x01, /*     INPUT (Cnst,Ary,Abs)             */
	0x05, 0x01, /*     USAGE_PAGE (Generic Desktop)     */
	0x09, 0x30, /*     USAGE (X)                        */
	0x09, 0x31, /*     USAGE (Y)                        */
	0x15, 0x81, /*     LOGICAL_MINIMUM (-127)           */
	0x25, 0x7f, /*     LOGICAL_MAXIMUM (127)            */
	0x75, 0x08, /*     REPORT_SIZE (8)                  */
	0x95, 0x02, /*     REPORT_COUNT (2)                 */
	0x81, 0x06, /*     INPUT (Data,Var,Rel)             */
	0xc0,       /*   END_COLLECTION                     */
	0xc0        /* END_COLLECTION                       */
};

static const struct usb_endpoint_descriptor hid_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,	/* bit[7]=1b -> direction: IN; bit[3:0]=001b -> endpoint no. 1 */
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = HID_DATA_PACKET_SIZE,
	.bInterval = 20,			/* polling interval in frames (USB2.0: 1 frame = 1ms) -> 20ms */
};

static const struct {
	struct usb_hid_descriptor hid_descriptor;
	struct {
		uint8_t bReportDescriptorType;
		uint16_t wDescriptorLength;
	} __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
	.hid_descriptor = {
		.bLength = sizeof(hid_function),
		.bDescriptorType = USB_DT_HID,
		.bcdHID = 0x0100,
		.bCountryCode = 13, /* International (ISO) */
		.bNumDescriptors = 1,
	},
	.hid_report = {
		.bReportDescriptorType = USB_DT_REPORT,
		.wDescriptorLength = sizeof(hid_report_descriptor),
	}
};

static const struct usb_interface_descriptor hid_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = USB_HID_SUBCLASS_BOOT_INTERFACE,
	.bInterfaceProtocol = USB_HID_INTERFACE_PROTOCOL_MOUSE,
	.iInterface = 0,

	.endpoint = &hid_endpoint,

	.extra = &hid_function,
	.extralen = sizeof(hid_function),
};

static const struct usb_interface iface = {
	.num_altsetting = 1,
	.altsetting = &hid_iface,
};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = USB_DT_CONFIGURATION_SIZE+USB_DT_INTERFACE_SIZE+USB_DT_ENDPOINT_SIZE+(sizeof(hid_function)),
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,	/* BUS powered Device */
	.bMaxPower = 0x64,		/* 0x64 -> 100d -> 100*2mA=200mA */

	.interface = &iface,
};

static const char *usb_strings[] = {
	"STM32 Maple Mini Clone (Baite)",
	"USB HID Mouse",
	"1.0.0",
};

usbd_device *usb_device;
static uint8_t usbd_control_buffer[128];

static enum usbd_request_return_codes hid_control_request_callback(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	if((req->bmRequestType != 0x81) ||
	   (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
	   (req->wValue != 0x2200))
		return USBD_REQ_NOTSUPP;

	/* Handle the HID report descriptor. */
	*buf = (uint8_t *)hid_report_descriptor;
	*len = sizeof(hid_report_descriptor);

	return USBD_REQ_HANDLED;
}

static void hid_set_config_callback(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)usbd_dev;
	(void)wValue;

	usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 4, ((void *)0));

	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				hid_control_request_callback);
}

/* The USB_autodetect()-function is used to re-enumerate the USB device on the PC,
so that after flashing, the device does not need to be physically reconnected. Additionally, 
USB D+ line (D23 = GPIOA, GPIO12) needs to be connected via a 1k5 Ohm pullup resistor to 
VCC (3.0 - 3.6V) to be compliant with the USB 2.0 full-speed electrical specification. */
static void USB_autodetect(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);

#if (USB_DP_1K5_PULLUP_SWITCHABLE_VIA_GPIOB9 == 1U)
    /* on the Maple Mini development board GPIOB 9 is used switch a transistor to dis-/connect an on-board
    1k5 Ohm pullup resistor to USB D+ line */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
	gpio_clear(GPIOB, GPIO9);
	delay_ms(250);
#else
    /* in case the 1k5 Ohm pullup resistor is permanently connected to USB D+ line:
    set GPIOA, GPIO12 (USB D+ line) to output push-pull low (0V) and wait for 250ms */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    delay_ms(250);
#endif
    
    /* set to alternate function (USB): */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12); /* USB D+ line */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO11); /* USB D- line */
    delay_ms(250);
}

void usb_hid_init(void)
{
	USB_autodetect();
    usb_device = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usb_device, hid_set_config_callback);
    nvic_set_priority(USB_IRQ, IRQ_PRI_USB);
	nvic_enable_irq(USB_IRQ);
}

void usb_hid_move_mouse_xy(int8_t x, int8_t y)
{
	struct mouse_report_data_t {
		uint8_t buttons;
		int8_t move_x;
		int8_t move_y;
	} __attribute__((packed)) mouse_report_data = {
		.buttons = 0, // 0=no, 1=left, 2=right, 4=middle
		.move_x = x,
		.move_y = y,
	};

	usbd_ep_write_packet(usb_device, 0x81, &mouse_report_data, sizeof(mouse_report_data));
}

void USB_ISR(void)
{
	usbd_poll(usb_device);
}
