#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


//Program entry point
int main() {
  //Initialize USB serial coms and wait for connection
  stdio_init_all();
  while( !stdio_usb_connected() ) sleep_ms(10);

  //Initialize and configure analog (ADC) input and GPIOs
  adc_init();
  adc_gpio_init(26);
  adc_select_input(0);
  
  for(int i = 0; i < 4; i++){
    gpio_set_dir(i, false);
    gpio_pull_up(i);
  }

  //Main loop
  while(true){
    //Poll the buttons
    for(int i = 0; i < 4; i++){
      if(!gpio_get(i)) printf("GPIO %d\n",i);
    }
    //Poll the ADC
    printf("Pot value: %d\n", adc_read());
    sleep_ms(250);
  }
  return 0;
}
