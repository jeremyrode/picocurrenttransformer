#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"

#define ADC_NUM 2
#define ADC_PIN (26 + ADC_NUM)
#define LED_PIN 25
#define MEAN_LOW_PASS_COEF .99999
#define SQ_LOW_PASS_COEF .99999
#define PRINT_RATIO 100000


int main() {
  // initialise GPIO (Green LED connected to pin 25)
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  //Initialise Standard I/O to USB
  stdio_usb_init();
  printf("Started Up\n");

  bi_decl(bi_program_description("JR Current Meter")); // for picotool
  bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));

  adc_init();
  adc_gpio_init(ADC_PIN);
  adc_select_input(ADC_NUM);

  int count = 1;
  float value, value_from_zero;
  float meanVal = 1.65;
  float meanSqVal = 0;
  while (1) {
    value = adc_read() * ( 3.3 / ((1 << 12) - 1)); // raw voltage from ADC
    //Keep Track of the DC offset
    meanVal = meanVal * MEAN_LOW_PASS_COEF + value * (1 - MEAN_LOW_PASS_COEF);
    // This is the squared value from the DC offset
    value_from_zero = value - meanVal;
    meanSqVal = meanSqVal * SQ_LOW_PASS_COEF + value_from_zero * value_from_zero * (1 - SQ_LOW_PASS_COEF);
    count++;
    if (count % PRINT_RATIO == 0) {
      printf("Mean: %5.6f\tSquared: %7.6f \n", meanVal, meanSqVal);
      gpio_put(LED_PIN, !gpio_get(LED_PIN)); // Toggle LED
    }
  }
}
