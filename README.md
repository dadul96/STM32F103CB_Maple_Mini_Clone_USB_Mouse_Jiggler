# STM32F103CB_Maple_Mini_Clone_USB_Mouse_Jiggler
 USB mouse jiggler with STM32F103CB-based Maple Mini Clone.

### Folder Structure
```
--STM32F103CB_Maple_Mini_Clone_USB_Mouse_Jiggler
  |
  |--lib
  |  |
  |  |--usb_hid_lib
  |     |
  |     |- delay.c
  |     |- delay.h
  |     |- usbhid.c
  |     |- usbhid.h
  |
  |--src
  |   |
  |   |- main.c
  |
  |- platformio.ini
```

### Code Dependency
```
           [main.c]
               |
               |
           [usbhid.*]
               |
               |
           [delay.*]
```

### Built With
* [PlatformIO IDE](https://platformio.org/platformio-ide) - IDE used for programming
* [libopencm3](https://github.com/libopencm3/libopencm3) - Open-Source lowlevel hardware library for ARM Cortex-M microcontrollers

### Author
**Daniel Duller** - [dadul96](https://github.com/dadul96)

### License
This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details

### References/Documentation/Inspiration
* [libopencm3 USB HID example](https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f1/other/usb_hid/usbhid.c)
* [satoshinm/pill_duck](https://github.com/satoshinm/pill_duck)
* [Missing USB D+ pullup workaround](https://hackaday.io/project/19799-lifetime-fails-project/log/160352-missing-external-usb-d-pullup-on-stm32-powered-pcb/discussion-163951)
* [LeafLabs Maple Mini Documentation](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/hardware/maple-mini.html)
* [stm32-base.org: Baite Maple Mini Clone Documentation](https://stm32-base.org/boards/STM32F103C8T6-Baite-Maple-Mini-Clone)
* [beyondlogic.org: USB Descriptors](https://beyondlogic.org/usbnutshell/usb5.shtml)
* [ST AN4879 USB hardware and PCB guidelines](https://www.st.com/content/ccc/resource/technical/document/application_note/group0/0b/10/63/76/87/7a/47/4b/DM00296349/files/DM00296349.pdf/jcr:content/translations/en.DM00296349.pdf)
* [Frank Zhao (eleccelerator.com): USB HID report descriptors](https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/)
* [USB.org: HID information](https://www.usb.org/hid)
* [USB.org: HID 1.11 device class definition](https://www.usb.org/document-library/device-class-definition-hid-111)
* [USB.org: HID usage tables](https://usb.org/document-library/hid-usage-tables-13)
* [Embedded USB Design By Example by John Hyde (Commissioned by FTDI Ltd)](https://mikrokontroler.pl/wp-content/uploads/pliki/FTDI_Book_C_0.pdf)
* [Stackoverflow: meaning of "\_\_attribute__((packed))"](https://stackoverflow.com/questions/11770451/what-is-the-meaning-of-attribute-packed-aligned4)
