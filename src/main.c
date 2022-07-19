#include "usbhid.h"

int main(void)
{
	usb_hid_init();

	while (1)
	{
		delay_ms(30000);
		usb_hid_move_mouse_xy(1, 0);
		delay_ms(30000);
		usb_hid_move_mouse_xy(-1, 0);
	}		
}
