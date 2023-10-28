#ifndef __USB_EP_H
#define __USB_EP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"
#include "usb_mem.h"
#include "usb.h"

#define numEP 8
#define USB_RX_BUF_COUNT 0x8400
#define USB_EP_BUFSIZE 64

typedef enum USB_EpType_e {
	CONTROL,
	BULK,
	INTERUPT,
	ISOCHRONOUS,
	EP_TYPENUMBERS
} USB_EpType;

typedef struct USB_EP_s {
	void (*SetEPR)(struct USB_EP_s* this, uint16_t mask, uint16_t data);
	void (*TransmitStall)(struct USB_EP_s* this);
	void (*TransmitPacket)(struct USB_EP_s* this, const uint8_t* data, size_t length);
	void (*ReceivePacket)(struct USB_EP_s* this, size_t length);
	void (*ReadBuffer)(struct USB_EP_s* this, uint8_t* buffer, size_t length);
	void (*OnTransmit)(struct USB_EP_s* this);
	void (*OnReceive)(struct USB_EP_s* this);
	void (*OnReset)(struct USB_EP_s* this);
	void (*SendStatus)(struct USB_EP_s* this, ErrorStatus status);

	uint16_t*  	epr;
	uint8_t    	eprNb;
	EP_BufDesc*	Descriptor;
	uint8_t    	address;

	UsbMem_t*  txBuffer;
	size_t     txBufferLen;
	UsbMem_t*  rxBuffer;
	size_t     rxBufferLen;
	USB_EpType type;

	uint_fast8_t error;
} USB_EP;

USB_EP USB_EP_Constructor(	uint8_t eprNb,
                        	uint8_t address,
                        	USB_EpType type,
                        	UsbMem_t* txBuffer,
                        	UsbMem_t* rxBuffer
                    );


typedef void (*USB_EP_SpecializeFct)(USB_EP*);

extern ALIGN(8) EP_BufDesc BufDescTable[8] USB_MEM;
extern ALIGN(4) UsbMem_t USB_EpBuffers[EP_TYPENUMBERS][USBMEM_SIZE(USB_EP_BUFSIZE)] USB_MEM;

void EPControl_Specialize(USB_EP* base);

#ifdef __cplusplus
}
#endif

#endif /* __USB_EP_H */
