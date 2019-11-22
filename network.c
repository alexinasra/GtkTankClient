#include "./network.h"

void send_wheel_speed (int left, int right) {
  float data[2];
  char bufptr[255];
  data[0] = left / 100.0;
  data[1] = right / 100.0;
  CURL *curl = curl_easy_init();

  sprintf(bufptr,"direction=(%f,%f)\0", data[0], data[1]);
  curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.4.1/drive");
  curl_easy_setopt(curl, CURLOPT_POST, 1L);

  /* size of the POST data */
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(bufptr));

  /* pass in a pointer to the data - libcurl will not copy */
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bufptr);

  int ret = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
}

void send_front_led_state (int state) {
  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.4.1/front_led");
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  char bufptr[255];
    sprintf(bufptr,"state=%s\0", state ? "on" : "off");
  /* size of the POST data */
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(bufptr));

  /* pass in a pointer to the data - libcurl will not copy */
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bufptr);

  int ret = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

}

