#include "./vehicle.h"

int led_state = FRONT_LED_OFF;

int left_speed_old = 0, right_speed_old = 0;


int forward_pressed = 0;
int backward_pressed = 0;
int left_pressed = 0;
int right_pressed = 0;
int handbreak_pressed = 0;

int gear = GEAR_1;

void gear_up_key_pressed () {

}

void gear_up_key_released () {
  if (gear < GEAR_3)
    gear++;
}
void gear_down_key_pressed() {

}

void gear_down_key_released() {
  if (gear > GEAR_1)
    gear--;
}

void forward_key_pressed () {
  forward_pressed = 1;
}

void forward_key_released () {
  forward_pressed = 0;
}

void backward_key_pressed () {
  backward_pressed = 1;
}
void backward_key_released () {
  backward_pressed = 0;
}

void left_key_pressed () {
  left_pressed = 1;
}
void left_key_released () {
  left_pressed = 0;
}

void right_key_pressed () {
  right_pressed = 1;
}
void right_key_released () {
  right_pressed = 0;
}

void handbreak_key_pressed () {
  handbreak_pressed = 1;
}
void handbreak_key_released () {
  handbreak_pressed = 0;
}

void frontled_key_pressed() {
  led_state = !led_state;
  send_front_led_state (led_state);
}
void frontled_key_released() {}


void *driver_thread (void *ptr) {
  OnVehicleUpdate *callback = (OnVehicleUpdate*) &ptr;
  int left_speed = 0, right_speed = 0;
  g_print("thread start\n");
  while (1) {
    left_speed = 0;
    right_speed = 0;
    if (gear == GEAR_1) {
      if (forward_pressed) {
        if (right_pressed) {
          left_speed = 10;
          right_speed = 90;
        } else if(left_pressed) {
          left_speed = 90;
          right_speed = 10;
        } else {
          left_speed = 10;
          right_speed = 10;
        }
      } else if(backward_pressed) {
        if (right_pressed) {
          left_speed = -10;
          right_speed = -90;
        } else if(left_pressed) {
          left_speed = -90;
          right_speed = -10;
        } else {
          left_speed = -10;
          right_speed = -10;
        }
      } else {
        if (left_pressed) {
          left_speed = 10;
          right_speed = -10;
        } else if (right_pressed) {
          left_speed = -10;
          right_speed = 10;
        }
      }
    } else if (gear == GEAR_2) {
      if (forward_pressed) {
        if (right_pressed) {
          left_speed = 10;
          right_speed = 90;
        } else if(left_pressed) {
          left_speed = 90;
          right_speed = 10;
        } else {
          left_speed = 50;
          right_speed = 50;
        }
      } else if(backward_pressed) {
        if (left_pressed) {
          left_speed = -10;
          right_speed = -90;
        } else if(right_speed) {
          left_speed = -90;
          right_speed = -10;
        } else {
          left_speed = -50;
          right_speed = -50;
        }
      } else {
        if (left_pressed) {
          left_speed = 10;
          right_speed = -10;
        } else if (right_pressed) {
          left_speed = -10;
          right_speed = 10;
        }
      }
    } else if (gear == GEAR_3) {
      if (forward_pressed) {
        if (right_pressed) {
          left_speed = 10;
          right_speed = 90;
        } else if(left_pressed) {
          left_speed = 90;
          right_speed = 10;
        } else {
          left_speed = 90;
          right_speed = 90;
        }

      } else if(backward_pressed) {
        if (right_pressed) {
          left_speed = -10;
          right_speed = -90;
        } else if(left_pressed) {
          left_speed = -90;
          right_speed = -10;
        } else {
          left_speed = -90;
          right_speed = -90;
        }
      } else {
        if (left_pressed) {
          left_speed = 10;
          right_speed = -10;
        } else if (right_pressed) {
          left_speed = -10;
          right_speed = 10;
        }
      }
    }
    if ( left_speed_old != left_speed || right_speed_old != right_speed) {
      send_wheel_speed (left_speed, right_speed);
      g_print("left: %d\tright: %d\n", left_speed, right_speed);
      //send data
      left_speed_old = left_speed;
      right_speed_old = right_speed;
      struct vehicle_state state = {
        left_speed,
        right_speed,
        gear
      };
      (*callback)(state);
      //gdk_threads_add_idle(callback, state);
    }
    usleep(BUTTON_RELEASE_DELAY);
  }
  return NULL;
}

int is_front_led_on() {
  return led_state != FRONT_LED_OFF;
}


