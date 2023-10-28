#ifndef __USB_EP_CONTROL_H
#define __USB_EP_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum USB_Enumeration_e {
	FIRST_DESCRIPTOR_STAGE,
	ADDRESS_STAGE,
	SECOND_DESCRIPTOR_STAGE,
	CONFIGURATION_DESCRIPTOR_STAGE
} USB_Enumeration;

#ifdef __cplusplus
}
#endif
#endif /* !__USB_EP_CONTROL_H */

