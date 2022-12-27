#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "hardware.h"

//============================================================
//  Internal Function Prototypes
//============================================================

uint16_t get_twist_16();
uint8_t from_16bit_to_8bit(uint16_t input);

//============================================================
//  Constants
//============================================================

//Twist it ADC ranges
enum {
  TWIST_MIN = 940,
  TWIST_DEAD_MIN = 1730,
  TWIST_DEAD_MAX = 1880,
  TWIST_MAX = 2490
};

//Math constants to map twist ranges to joystick ranges (-1 to 1 range mapped to 16bit unsigned range)
constexpr float NEG_SLOPE = 41.477;
constexpr float POS_SLOPE = 53.718;
constexpr float NEG_OFFSET = -38988.582;
constexpr float POS_OFFSET = -68222.902;
constexpr float TWIST_OUT_MAX = 65535;
constexpr float TWIST_OUT_MID = 32767;

//Hardware wiring constants
enum {
  BOP_IT_BUTTON = 0,
  PULL_IT_BUTTON = 1,
  VOLUME_BUTTON = 3,
  SLIDER_BUTTON = 2,
  LED_PIN = PICO_DEFAULT_LED_PIN
};


//============================================================
//  General interface functions 
//============================================================

//Initialize all the hardware
void hardware_init(){
  //ADC init
  adc_init();
  adc_gpio_init(26);
  adc_select_input(0);

  //Initialize button GPIOs
  for (int i = 0; i < 4; i++){
    gpio_init(i);
    gpio_set_dir(i, GPIO_IN);
    gpio_pull_up(i);
  }

  //Initialize LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
}

//Return the twist value as a signed 8 bit value
int8_t get_twist(){
  return from_16bit_to_8bit(get_twist_16())-128;
}

//Return the button values as one 8 bit binary number (bottom 4 bits are asigned to each button)
uint8_t get_buttons(){
  return (!gpio_get(BOP_IT_BUTTON) << 0 |
      !gpio_get(PULL_IT_BUTTON) << 1 |
      !gpio_get(VOLUME_BUTTON) << 2 |
      !gpio_get(SLIDER_BUTTON) << 3);
}

//Blink the led at the given interval
void led_blinking_task(uint32_t& blink_interval_ms) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  //Blink is disabled
  if (!blink_interval_ms) return;

  //Blink every interval ms
  if ( to_ms_since_boot(get_absolute_time()) - start_ms < blink_interval_ms) return; //Not enough time
  start_ms += blink_interval_ms;

  gpio_put(LED_PIN, led_state);
  led_state = !led_state; //Toggle
}


//============================================================
//  Internal hardware interfaces 
//============================================================

//Return the twist mapped from the raw adc to a unsigned 16 bit number
uint16_t get_twist_16(){
  //Read twist
  uint16_t val = adc_read();

  /* Essentially a piecewise function for negative rotation, deadzone, and positive
   *        | 0                            x < TWIST_MIN
   *        | NEG_SLOPE * x + NEG_OFFSET   TWIST_MIN  <= x < TWIST_DEAD_MIN
   * f(x) = | TWIST_OUT_MID                TWIST_DEAD_MIN <= x <= TWIST_DEAD_MAX
   *        | POS_SLOPE * x + POS_OFFSET   TWIST_DEAD_MAX < x <= TWIST_MAX
   *        | TWIST_OUT_MAX                TWIST_MAX < x
   */

  if (val < TWIST_MIN) {
    return 0;
  }
  else if (val >= TWIST_MIN && val < TWIST_DEAD_MIN) {
    return NEG_SLOPE * val + NEG_OFFSET;
  }
  else if (val >= TWIST_DEAD_MIN && val <= TWIST_DEAD_MAX) {
    return TWIST_OUT_MID;
  }
  else if (val > TWIST_DEAD_MAX && val <= TWIST_MAX) {
    return POS_SLOPE * val + POS_OFFSET;
  }
  else return TWIST_OUT_MAX;
}

//Scale a 16 bit integer to an 8 bit integer
uint8_t from_16bit_to_8bit(uint16_t input){
  uint16_t output;
  
  //Rounded divide by 256
  output = (input + 128)/256;
  if (output == 256) output--;

  return (uint8_t) output;
}


