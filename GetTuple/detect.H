#ifndef _PCMCIATESTDLL_H
#define _PCMCIATESTDLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <winioctl.h>

#define IOCTL_PCMCIA_TEST_TUPLES     CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#ifdef __cplusplus
}
#endif

#endif	/* _PCMCIATESTDLL_H */
