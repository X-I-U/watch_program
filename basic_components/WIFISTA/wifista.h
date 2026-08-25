#ifndef __WIFISTA_H_
#define __WIFISTA_H_

#include <stdbool.h> 

#define DEFAULT_SSID "mywifi"
#define DEFAULT_PWD  "fxc520146"

void wifista_init(void);
bool wifi_wait_connected(int timeout_ms);

#endif
