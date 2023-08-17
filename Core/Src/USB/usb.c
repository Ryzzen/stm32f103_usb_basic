#include "usb.h"
#include "usb_def.h"
#include <stdint.h>

static USB_EP endpoints[EP_TYPENUMBERS];

USB_DevDesc devDesc = {
        18,
        1,
        0x0200,
        0xff,
        0xff,
        0xff,
        64,
        0xdead,
        0xbeef,
        0x0100,
        0,
        0,
        0,
        1
    };

static void USB_EndpointsInit()
{
    endpoints[CONTROL] = USB_EP_Constructor(0, 0, CONTROL, USB_EpBuffers[0], USB_EpBuffers[0]);
}

// TODO: Use a counter/timer to add a real delay function
static void delay()
{
    for (uint32_t i = 0 ; i < 720000 ; ++i) __NOP();
}

/* 
    Little dance to have our device detected.
    Since we do not have a programmable resistor,
    we force D+ to low.
*/
static void HardwareReset()
{
    // Set PA12 as open drained  output
    GPIOA->CRH &= ~(0b1111 << 16);
    GPIOA->CRH |= (0b0111 << 16);

    GPIOA->ODR &= ~(1 << 12);   // Pull PA12 low (D+ resistor)

    delay();

    GPIOA->CRH &= ~(0b1111 << 12);  // PA12 reset at analog input
}

static void USB_SuspInterruptHandler()
{
    USB->ISTR &= ~USB_ISTR_SUSP;
}

void USB_ResetInterruptHandler()
{
    USB->BTABLE = MapAddr(BufDescTable);

    USB->DADDR = USB_DADDR_EF | (0 << USB_DADDR_ADD_Pos);

    USB_EndpointsInit();

    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM;

    endpoints[CONTROL].OnReset(&endpoints[CONTROL]);

    //  Clear reset interrupt
    USB->ISTR &= ~USB_ISTR_RESET;
}

static void USB_CTRInterruptHandler()
{   
    uint16_t ISTR = USB->ISTR;

    uint8_t dir = ISTR & USB_ISTR_DIR;
    uint8_t ep = (ISTR & USB_ISTR_EP_ID) >> USB_ISTR_EP_ID_Pos;
    uint16_t epr = *endpoints[ep].epr;

    //  Reset flags right after sampling them so the hardware can update them right away for the next itteration
    endpoints[ep].SetEPR(&endpoints[ep], (USB_EP0R_CTR_RX | USB_EP0R_CTR_TX), 0);    

    if (dir && (epr & USB_EP0R_CTR_RX)) {
		// Receive packet... 
        endpoints[ep].OnReceive(&endpoints[ep]);
	}

    if (epr & USB_EP0R_CTR_TX) {
		// packet sent...
        endpoints[ep].OnTransmit(&endpoints[ep]);
    }
    USB->ISTR &= ~USB_ISTR_CTR;
}

void USB_LP_CAN1_RX0_IRQHandler()
{
    const uint16_t isUSBInterrupt = USB_ISTR_CTR | USB_ISTR_RESET | USB_ISTR_SUSP;
    uint16_t ISTR;  // Adding a tmp variabe to keep atomic the access to the register
    
     while (((ISTR = USB->ISTR) & isUSBInterrupt)) {
        if (ISTR & USB_ISTR_RESET)      // RESET interrupt handle
            USB_ResetInterruptHandler();
        if (ISTR & USB_ISTR_CTR)        // CTR interrupt handle
            USB_CTRInterruptHandler();
        if (USB->ISTR & USB_ISTR_SUSP)  // Suspend interrupt handle
            USB_SuspInterruptHandler();
    }
}

void USB_Init()
{
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
    
    HardwareReset();    // For dev purposes, so I don't have to manualy unplug the device before each debug session

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    USB->CNTR = 0;
    USB->BTABLE = 0;
    USB->DADDR = 0;
    USB->ISTR = 0;

    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void USB_Connect()
{
    USB->CNTR = USB_CNTR_FRES;  // USB Force reset | Set PDWN to low

    delay();    // 1micro second delay (tSTARTUP) to wait to for the analog circuit to come to life

    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_SUSPM;    // Enable Correct Transfer interrupt and Reset interrupt

    USB->ISTR = 0;
    NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
}
