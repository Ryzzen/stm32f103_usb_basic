#include "usb_ep.h"

static __IO UsbMem_t* EPnR = (UsbMem_t*)(USB_BASE); // EPnR registers, array of size numEP

ALIGN(8) EP_BufDesc BufDescTable[8] USB_MEM = {0};
ALIGN(4) UsbMem_t USB_EpBuffers[EP_TYPENUMBERS][USBMEM_SIZE(USB_EP_BUFSIZE)] USB_MEM = {0};


void SetEPR(USB_EP* this, uint16_t mask, uint16_t data)
{
    uint16_t old = *this->epr;
    uint16_t rc_w0 = USB_EP_CTR_RX_Msk | USB_EP_CTR_TX_Msk; // These bits are cleared when writing 0 and remain unchanged when 1.
    uint16_t toggle = USB_EP_DTOG_RX_Msk | USB_EPRX_STAT_Msk | USB_EP_DTOG_TX_Msk | USB_EPTX_STAT_Msk;  // These bits are toggled on writing from 1, and remain unchanged on 0. 
    uint16_t rw = USB_EP_T_FIELD_Msk | USB_EP_KIND_Msk | USB_EPADDR_FIELD;  // These bits behave "normally", ie the written value is accepted directly. 

    uint16_t wr0 = rc_w0 & (~mask | data);  // Check bits to clear
    uint16_t wr1 = (mask & toggle) & (old ^ data);  // For bits with toggle behavior, the old state must be considered and processed via XOR 
    uint16_t wr2 = rw & ((old & ~mask) | data); // With "normal" bits, the old state is retained or overwritten if desired. 

    *this->epr = (uint16_t)(wr0 | wr1 | wr2);    // Combine all three writing methods.;
}

void EPn_TransmitStall(USB_EP* this)
{
	if (!this)
		return;

	this->SetEPR(this, USB_EPTX_STAT, USB_EP_TX_STALL);
}

void EPn_TransmitPacket(USB_EP* this, const uint8_t* data, size_t length)
{
    if (!this) {
        return;
    }

	if (length && data) {
		for (uint_fast16_t i = 0; (i < length) || (i < this->txBufferLen);) {
			*(uint16_t*)(USBMEM_BYTE_ADR(this->txBuffer, i)) = \
			(i + 1 >= length) ?\
			(uint16_t)(data[i]) :\
			HW_TO_W(data[i + 1], data[i]);
			i += 2;
		}
	    this->Descriptor->txBufferAddr = MapAddr(this->txBuffer);
	} else {
	    this->Descriptor->txBufferAddr = 0;
	}

	this->Descriptor->txBufferCount = length;
    this->SetEPR(this, USB_EPTX_STAT, USB_EP_TX_VALID);
}

void EPn_ReceivePacket(USB_EP* this, size_t length)
{
    if (!this || !ASSERT_USBMEM_SIZE(length) || (length >= this->txBufferLen)) {
        return;
	}

	if (!length) {
		this->Descriptor->rxBufferCount = 0;
		this->SetEPR(this, USB_EPRX_STAT | USB_EP_KIND, USB_EP_RX_VALID | USB_EP_KIND);
		return;
	}

	uint16_t BL_SIZE = !(length <= 62);
	uint16_t NUM_BLOCK = BL_SIZE ? (uint16_t) ((length/32)-1) : (uint16_t)(length/2);

    this->Descriptor->rxBufferAddr = MapAddr(this->rxBuffer);
    this->Descriptor->rxBufferCount = (uint16_t)((BL_SIZE << 15) | (NUM_BLOCK << 10));

    this->SetEPR(this, USB_EPRX_STAT, USB_EP_RX_VALID);
}

void EPn_OnTransmit(USB_EP* this)
{
    if (!this)
        return;

    EPn_ReceivePacket(this, this->rxBufferLen);
}

void EPn_OnReceive(USB_EP* this)
{
    return;
}

void EPn_OnReset(USB_EP* this)
{
    return;
}

void EPn_ReadBuffer(USB_EP* this, uint8_t* buffer, size_t length)
{
    if (!this || !buffer)
        return;

     for (uint_fast32_t i = 0; (i < length) && (i < this->rxBufferLen) ; i++)
        buffer[i] = USBMEM_BYTE(this->rxBuffer, i);
}

void Specialize(USB_EP* base)
{
    if (!base)
        return;

    static USB_EP_SpecializeFct epFactory[EP_TYPENUMBERS] = {
        &EPControl_Specialize
    };

    epFactory[base->type](base);
}

USB_EP USB_EP_Constructor(  uint8_t eprNb,
                            uint8_t address,
                            USB_EpType type,
                            UsbMem_t* txBuffer,
                            UsbMem_t* rxBuffer
                    )
{
    USB_EP result = {
        &SetEPR,
        &EPn_TransmitStall,
        &EPn_TransmitPacket,
        &EPn_ReceivePacket,
        &EPn_ReadBuffer,
        &EPn_OnTransmit,
        &EPn_OnReceive,
        &EPn_OnReset,
        (uint16_t*)(USBMEM_BYTE_ADR(EPnR, (eprNb * 2))),
        eprNb,
        &(BufDescTable[eprNb]),
        address,
        txBuffer,
        USB_EP_BUFSIZE,
        rxBuffer,
        USB_EP_BUFSIZE,
        type
    };
    
    Specialize(&result);

    return result;
}
