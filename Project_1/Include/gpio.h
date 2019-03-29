#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define GPIO_DIR "/sys/class/gpio"
#define GPIO_LED 53
#define GPIO_LIGHT 48

timer_t blink_timer_id;

int gpio_export(uint32_t pin);
int gpio_dir(uint32_t pin);
int gpio_set_value(uint32_t pin, uint32_t val);
int gpio_get_value(uint32_t pin);
int gpio_edge(uint32_t pin, char *edge);
FILE *gpio_open(uint32_t pin, char *file);
int gpio_close(FILE *fp);
int gpio_blink(uint32_t pin);
void blink_timer_init(uint32_t pin);
void blink_timer_handle(union sigval sv);
