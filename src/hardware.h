#ifndef HARDWARE
#define HARDWARE

/* Header for the hardware
 * 
 * hardware_init() Starts all the hardware
 * get_twist() returns the axis value for the twist
 * get_buttons() returns the button states
 * led_blinking_task(...) is a periodic task to indicate status through the onboard LED
 */

void hardware_init();
int8_t get_twist();
uint8_t get_buttons();
void led_blinking_task(uint32_t& blink_interval_ms);

#endif
