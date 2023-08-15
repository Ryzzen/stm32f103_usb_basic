#include "usb_mem.h"

//ALIGN(8) EP_BufDesc BufDescTable[8] USB_MEM = {0};

uint16_t MapAddr(void* addr)
{ 
    extern uint8_t susbbuf; // Starting address of the usbbuf section, define in the linker
    return (uint16_t)(((uintptr_t)(addr) - (uintptr_t)(&susbbuf)) / 2); // Subtract address from starting address and divide by 2 to remove padding. 
}