
#ifndef USB_H_
#define USB_H_

void usb_init(void);
void usb_handle_systick(void);
void usb_handle_data(void);
bool usb_enabled(void);

#endif  // USB_H_

