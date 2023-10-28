#include "usb_ep_control.h"
#include "usb_ep.h"
#include "usb.h"
#include <stdint.h>

static void EPContol_SetupStage(USB_EP* this)
{
	if (!this) {
		return;
	}

    uint8_t rxData[sizeof(USB_DevRequest)];
    this->ReadBuffer(this, rxData, sizeof(USB_DevRequest));

    USB_DevRequest* req = (USB_DevRequest*)(rxData);

	uint8_t type = (req->wValue >> 8);
	uint8_t index = req->wValue;

    if (req->bmRequestType == 0x80 && req->bRequest == GET_DESCRIPTOR) {
        if (type == DEVICE && index == 0 && req->wIndex == 0) {
            uint8_t txData[sizeof(devDesc)];

            txData[0] = devDesc.bLength;
            txData[1] = devDesc.bDescriptorType;
            txData[2] = devDesc.bcdUSB;
            txData[3] = devDesc.bcdUSB >> 8;
            txData[4] = devDesc.bDeviceClass;
            txData[5] = devDesc.bDeviceSubClass;
            txData[6] = devDesc.bDeviceProtocol;
            txData[7] = devDesc.bMaxPacketSize0;
            txData[8] = devDesc.idVendor;
            txData[9] = devDesc.idVendor >> 8;
            txData[10] = devDesc.idProduct;
            txData[11] = devDesc.idProduct >> 8;
            txData[12] = devDesc.bcdDevice;
            txData[13] = devDesc.bcdDevice >> 8;
            txData[14] = devDesc.iManufacturer;
            txData[15] = devDesc.iProduct;
            txData[16] = devDesc.iSerialNumber;
            txData[17] = devDesc.bNumConfigurations;

            this->TransmitPacket(this, txData, sizeof(txData)); 
        }
    }

	if (req->bmRequestType == 0x00 && req->bRequest == SET_ADDRESS) {
		this->address = (uint8_t)(req->wValue & 0x7F);
		this->SendStatus(this, SUCCESS);
	}
}

void EPControl_StatusStage(USB_EP* this)
{
	if (this->address) {
		USB_SETADDRESS(this->address);
		this->address = 0;
	}
	/* this->ReceivePacket(this, this->rxBufferLen); */
}

void EPControl_OnReceive(USB_EP* this)
{
    if (!this) {
        return;
	}

	uint16_t isSetup = *(this->epr) & USB_EP_SETUP;

	EPControl_StatusStage(this);
	EPContol_SetupStage(this);
}

void EPControl_OnReset(USB_EP* this)
{
    if (!this)
        return;

    this->Descriptor->rxBufferAddr = MapAddr(this->rxBuffer);
    this->Descriptor->rxBufferCount = USB_RX_BUF_COUNT;

    this->SetEPR(this,
        USB_EPRX_STAT_Msk | USB_EP_T_FIELD_Msk | USB_EPADDR_FIELD | USB_EP_KIND | USB_EPTX_STAT_Msk,
        (uint16_t)(USB_EP_RX_VALID | USB_EP_TX_NAK | USB_EP_CONTROL));
}

void EPControl_Specialize(USB_EP* base)
{
    if (!base)
        return;

    base->OnReset = &EPControl_OnReset;
    base->OnReceive = &EPControl_OnReceive;
}
