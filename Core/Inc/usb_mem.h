#ifndef __USB_MEM_H
#define __USB_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"

#define USB_MEM     __attribute__((section(".usbbuf")))
#define ALIGN(n)    __attribute__((aligned(n)))

#define USBMEM_BYTE_ADR(buf, i)     (((uint8_t*)(buf) + (i * 2) - (i % 2)))    // Get byte, skipping padding
#define USBMEM_BYTE(buf, i)         (*USBMEM_BYTE_ADR(buf, i))
#define ASSERT_USBMEM_SIZE(size)    ((size > 0) && ((( size <= 62 ) && ( size % 2 == 0 )) || (( size <= 512 ) && ( size % 32 == 0 ))))
#define USBMEM_SIZE(size)           (ASSERT_USBMEM_SIZE(size) ? size / 2 : -1)
#define HW_TO_W(hByte, lByte)       ((uint16_t)(((hByte & 0xff) << 8) | (lByte & 0xff)))
//#define EPnR_ARR(i)                 HW_TO_W(USBMEM_BYTE(EPnR, i + 1), USBMEM_BYTE(EPnR, i))

#define numEP 8

typedef struct EP_BufDesc_s {  
    uint16_t txBufferAddr;	
	uint16_t Padding1; 

    uint16_t txBufferCount;	 
	uint16_t Padding2; 

    uint16_t rxBufferAddr;	 
	uint16_t Padding3; 

    uint16_t rxBufferCount;	 
	uint16_t Padding4; 
} EP_BufDesc;

// Device descriptor
typedef struct USB_DevDesc_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} USB_DevDesc;

// Request issued by a SETUP from host
typedef struct USB_DevRequest_s {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_DevRequest;

/*
    USB buffer addresses are 32 bits aligned
    due to APB bridge only able to access 32bits words,
    so we work with 2 bytes of usable variables
    and 2 bytes of padding per word.
*/
typedef uint32_t UsbMem_t;

//extern ALIGN(8) EP_BufDesc BufDescTable[8] USB_MEM;

uint16_t MapAddr(void* addr);

#ifdef __cplusplus
}
#endif

#endif /* __USB_MEM_H */