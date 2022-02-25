#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define GREEN_LED 15
#define BLUE_LED 14
#define RED_LED 13

int gColor = 255;
int rColor = 205;
int bColor = 120;
// int colorValue;
char buffer[16];

void rgbInterrupt()
{
  while (multicore_fifo_rvalid())
  {
    char newColorValue[9];
    int test = 0;
    test = multicore_fifo_pop_blocking();
    extern char colorValue[9];
    if (newColorValue != colorValue)
    {
      // colorValue = newColorValue;
      sscanf(newColorValue, "%02x%02x%02x", &rColor, &gColor, &bColor);
      pwm_set_gpio_level(GREEN_LED, gColor * gColor);
      pwm_set_gpio_level(RED_LED, rColor * rColor);
      pwm_set_gpio_level(BLUE_LED, bColor * bColor);
    }
  }
  multicore_fifo_clear_irq();
}

void rgbThreadEntry()
{ // Thread for PWM/RGB-Control
  int colorValue;
  char newColorValue[8];

  // Tell the LED pin that the PWM is in charge of its value.
  gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);
  gpio_set_function(BLUE_LED, GPIO_FUNC_PWM);
  gpio_set_function(RED_LED, GPIO_FUNC_PWM);
  // Figure out which slice we just connected to the LED pin
  uint slice_num_g = pwm_gpio_to_slice_num(GREEN_LED);
  uint slice_num_r = pwm_gpio_to_slice_num(RED_LED);
  uint slice_num_b = pwm_gpio_to_slice_num(BLUE_LED);

  // Get some sensible defaults for the slice configuration. By default, the
  // counter is allowed to wrap over its maximum range (0 to 2**16-1)
  pwm_config config = pwm_get_default_config();
  // Set divider, reduces counter clock to sysclock/this value
  pwm_config_set_clkdiv(&config, 4.f);
  // Load the configuration into our PWM slice, and set it running.
  pwm_init(slice_num_b, &config, true);
  pwm_init(slice_num_r, &config, true);
  pwm_init(slice_num_g, &config, true);

  pwm_set_gpio_level(GREEN_LED, gColor * gColor);
  pwm_set_gpio_level(RED_LED, rColor * rColor);
  pwm_set_gpio_level(BLUE_LED, bColor * bColor);

  // We clear the interrupt flag, if it got set by a chance
  multicore_fifo_clear_irq();
  // set the SIO_IRQ_PROC1 (FIFO register set interrupt) ownership to only one core. Opposite to irq_set_shared_handler() function
  // We pass it the name of function that shall be executed when interrupt occurs
  irq_set_exclusive_handler(SIO_IRQ_PROC1, rgbInterrupt);
  // enable interrupt
  irq_set_enabled(SIO_IRQ_PROC1, true);
  while (1)
  {
    tight_loop_contents();
  }
}

int main()
{
  stdio_init_all();
  multicore_launch_core1(rgbThreadEntry);
  int userinput;
  while (true)
  {
    scanf("%x", &userinput);

    bColor = ((userinput) & 0xFF);
    gColor = ((userinput >> 8) & 0xFF);
    rColor = ((userinput >> 8) & 0xFF);
    multicore_fifo_push_blocking(3);
  }
}