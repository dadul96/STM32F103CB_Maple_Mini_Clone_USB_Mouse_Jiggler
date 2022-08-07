#if !defined(__USBHID_H__)
#define __USBHID_H__

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>

#include "delay.h"

#define USB_HID_PACKET_SIZE     (32U)
#define HID_DATA_PACKET_SIZE    (4U)
#define USB_IRQ                 NVIC_USB_LP_CAN_RX0_IRQ
#define USB_ISR                 usb_lp_can_rx0_isr
#define IRQ_PRI_USB             (255U)

/* 1 = resistor can be dis-/connected via a transistor and GPIOB9 signal; 0 = resistor is permanently connected: */
#define USB_DP_1K5_PULLUP_SWITCHABLE_VIA_GPIOB9 (1U)

void usb_hid_init(void);
void usb_hid_move_mouse_xy(int8_t x, int8_t y);

#endif /* __USBHID_H__ */
