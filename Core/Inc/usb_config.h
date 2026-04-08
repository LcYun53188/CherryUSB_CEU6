#ifndef CHERRYUSB_CONFIG_H
#define CHERRYUSB_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* ================= USB Common Configuration ================ */
#ifndef USB_DBG_LEVEL
#define USB_DBG_LEVEL 2 /* USB_DBG_INFO */
#endif

#define CONFIG_USB_PRINTF printf
#define usb_malloc(s) malloc(s)
#define usb_free(p) free(p)

#define USB_NOCACHE_RAM_SECTION
#define CONFIG_USB_ALIGN_SIZE 4

/* ================= USB Host Configuration ================ */
#define CONFIG_USBHOST_MAX_BUS              1
#define CONFIG_USBHOST_MAX_RHPORTS          1
#define CONFIG_USBHOST_MAX_EXTHUBS          1
#define CONFIG_USBHOST_MAX_EHPORTS          4
#define CONFIG_USBHOST_MAX_INTERFACES       6
#define CONFIG_USBHOST_MAX_ENDPOINTS        6
#define CONFIG_USBHOST_MAX_INTF_ALTSETTINGS 1

#define CONFIG_USBHOST_MAX_CDC_ACM_CLASS    0
#define CONFIG_USBHOST_MAX_HID_CLASS        4
#define CONFIG_USBHOST_MAX_MSC_CLASS        0
#define CONFIG_USBHOST_MAX_AUDIO_CLASS      0
#define CONFIG_USBHOST_MAX_VIDEO_CLASS      0
#define CONFIG_USBHOST_MAX_XBOX_CLASS       1

#define CONFIG_USBHOST_DEV_NAMELEN          16
#define CONFIG_USBHOST_PSC_PRIO             4
#define CONFIG_USBHOST_PSC_STACKSIZE        1024
#define CONFIG_USBHOST_REQUEST_BUFFER_LEN   512
#define CONFIG_USBHOST_CONTROL_TRANSFER_TIMEOUT 500

/* ================= USB Port Configuration ================ */
#define CONFIG_USB_DWC2_PORT USB_OTG_FS_PERIPH_BASE
#define USB_BASE USB_OTG_FS_PERIPH_BASE

/* FreeRTOS */
#define CONFIG_USBHOST_USE_FREERTOS
#define CONFIG_USB_OS_FREERTOS

#endif
