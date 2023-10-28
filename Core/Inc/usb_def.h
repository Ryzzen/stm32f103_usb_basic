
#ifndef __USB_DEF_H
#define __USB_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

// bRequest byte from devRequest
typedef enum USB_bRequest_e {
	GET_STATUS,
	CLEAR_FEATUR,
	RESERVED1,
	SET_FEATURE,
	RESERVED2,
	SET_ADDRESS,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	GET_CONFIGURATION,
	SET_CONFIGURATION,
	GET_INTERFACE,
	SET_INTERFACE,
	SYNCH_FRAME
} USB_bRequest;

// Descriptor type from wValue word of devRequest
typedef enum USB_bDescType_e {
	UNKNOWNED,
	DEVICE,
	CONFIGURATION,
	STRING,
	INTERFACE,
	ENDPOINT,
	DEVICE_QUALIFIER,
	OTHER_SPEED_CONFIGURATION,
	INTERFACE_POWER,
	OTG
} USB_bDescType ;

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEF_H */
