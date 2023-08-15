#include "usb_ep_control.h"
#include "usb_ep.h"

static USB_EpControlStage stage = STANDBY; 

static void  EPControl_SendStatus(USB_EP *this, uint_fast8_t status)
{
	if (status) {
		this->TransmitPacket(this, NULL, 0);
	} else {
		this->TransmitStall(this);
		this->ReceivePacket(this, this->rxBufferLen);
	}
	return;
}

static void EPControl_Handshake(USB_EP* this)
{
	
	return;
}

static void EPControl_DataIn(USB_EP* this) 
{ 
	return;
}

static void EPControl_DataOut(USB_EP* this)
{
	return;
}

void EPControl_OnReceive(USB_EP* this, uint_fast8_t setup)
{
    if (!this) {
        return;
	}

	switch (stage) {
		case STANDBY:
			break;
		case HANDSHAKE:
			EPControl_Handshake(this);
			break;
		case RECV_SETUP:
			EPControl_DataIn(this);
			break;
		case SEND_SETUP:
			EPControl_DataOut(this);
		break;
	}

    // Extract the parameters of the request uint8_t bmRequestType = static_cast < uint8_t > ( EP0_BUF [ 0 ] & 0xFF );		

    uint8_t rxData[sizeof(USB_DevRequest)];
    this->ReadBuffer(this, rxData, sizeof(USB_DevRequest));

    USB_DevRequest* req = (USB_DevRequest*)(rxData);

    if(req->bmRequestType == 0x80 && req->bRequest == 0x6) {
        uint8_t type = (req->wValue >> 8);
        uint8_t index = req->wValue;

        if (type == 1 && index == 0 && req->wIndex == 0) {
            uint8_t txData[sizeof(devDesc)];

            txData[0] = devDesc.bLength;
            txData[1] = devDesc.bDescriptorType;
            txData[2] = devDesc.bcdUSB >> 8;
            txData[3] = devDesc.bcdUSB;
            txData[4] = devDesc.bDeviceClass;
            txData[5] = devDesc.bDeviceSubClass;
            txData[6] = devDesc.bDeviceProtocol;
            txData[7] = devDesc.bMaxPacketSize0;
            txData[8] = devDesc.idVendor >> 8;
            txData[9] = devDesc.idVendor;
            txData[10] = devDesc.idProduct >> 8;
            txData[11] = devDesc.idProduct;
            txData[12] = devDesc.bcdDevice >> 8;
            txData[13] = devDesc.bcdDevice;
            txData[14] = devDesc.iManufacturer;
            txData[15] = devDesc.iProduct;
            txData[16] = devDesc.iSerialNumber;
            txData[17] = devDesc.bNumConfigurations;

            this->TransmitPacket(this, txData, sizeof(txData)); 
        }
    }
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
