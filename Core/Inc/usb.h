#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"
#include "usb_mem.h"
#include "usb_def.h"
#include "usb_ep.h"

#define USB_SETADDRESS(address) (USB->DADDR = (uint16_t)(USB_DADDR_EF | address))

void USB_Init();
void USB_Connect();

extern USB_DevDesc devDesc;

#ifdef __cplusplus
}
#endif

#endif /* __USB_H */
