#ifndef __DRIVER_H
#define __DRIVER_H
#include <glib.h>
#include <unistd.h>
#include "./network.h"

#define BUTTON_RELEASE_DELAY 100000

#define FRONT_LED_OFF 0
#define FRONT_LED_ON  !FRONT_LED_OFF

#define GEAR_1 1
#define GEAR_2 2
#define GEAR_3 3

struct vehicle_state {
  int left_speed;
  int right_speed;
  int gear;
};

#define PROJECT_NAME "GTKTankClient"
typedef void (*OnVehicleUpdate)(struct vehicle_state state);


void *driver_thread (void *ptr);

void gear_up_key_pressed ();
void gear_up_key_released ();
void gear_down_key_pressed();
void gear_down_key_released();


void forward_key_pressed();
void forward_key_released();

void backward_key_pressed();
void backward_key_released();

void left_key_pressed();
void left_key_released();

void right_key_pressed();
void right_key_released();

void handbreak_key_pressed();
void handbreak_key_released();

void frontled_key_pressed();
void frontled_key_released();

int is_front_led_on();

#endif


