#ifndef NETWORK_H
#define NETWORK_H 0

#include <curl/curl.h>

void send_wheel_speed(int left, int right);
void send_front_led_state (int state);


#endif
