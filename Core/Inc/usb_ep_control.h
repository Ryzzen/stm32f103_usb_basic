#ifndef __USB_EP_CONTROL_H
#define __USB_EP_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_ep.h"

typedef enum USB_EpControlStage_e {
	STANDBY,
	HANDSHAKE,
	RECV_SETUP,
	SEND_SETUP
} USB_EpControlStage;

#ifdef __cplusplus
}
#endif
#endif /* !__USB_EP_CONTROL_H */

