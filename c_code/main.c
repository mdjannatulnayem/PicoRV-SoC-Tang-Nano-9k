#include "leds.h"
#include "uart.h"
#include "countdown_timer.h"

#define TICKS_PER_SEC 5400000

enum TrafficState {
  STATE_GREEN = 0,
  STATE_YELLOW = 1,
  STATE_RED = 2
};

unsigned int green_delay = 5;  // default seconds
unsigned int yellow_delay = 2;
unsigned int red_delay = 5;

void uart_get_number(const char *prompt, unsigned int *val)
{
  char buf[16];
  int i = 0;
  char ch;

  uart_puts(prompt);

  while (1) {
    ch = uart_getchar();
    uart_putchar(ch); // echo
    if (ch == '\r' || ch == '\n') {
      break;
    }
    if (i < sizeof(buf) - 1) {
      buf[i++] = ch;
    }
  }

  buf[i] = '\0';
  *val = 0;

  for (int j = 0; j < i; j++) {
    if (buf[j] >= '0' && buf[j] <= '9') {
      *val = (*val * 10) + (buf[j] - '0');
    } else {
      uart_puts("\r\nInvalid input. Defaulting to 1 sec.\r\n");
      *val = 1;
      return;
    }
  }
}

void traffic_light_fsm()
{
  enum TrafficState state = STATE_GREEN;
  unsigned int delay_ticks;

  while (1) {
    switch (state) {
      case STATE_GREEN:
        set_leds(1); // Green
        uart_puts("\r\nState: GREEN\r\n");
        delay_ticks = green_delay * TICKS_PER_SEC;
        state = STATE_YELLOW;
        break;

      case STATE_YELLOW:
        set_leds(2); // Yellow
        uart_puts("\r\nState: YELLOW\r\n");
        delay_ticks = yellow_delay * TICKS_PER_SEC;
        state = STATE_RED;
        break;

      case STATE_RED:
        set_leds(4); // Red
        uart_puts("\r\nState: RED\r\n");
        delay_ticks = red_delay * TICKS_PER_SEC;
        state = STATE_GREEN;
        break;
    }

    cdt_delay(delay_ticks); // Blocking delay
  }
}

int main()
{
  set_leds(0xff);  // All LEDs on initially for visual boot indicator
  uart_set_div(47); // 5.4 MHz / 115200 baud

  uart_puts("\r\n=== Traffic Light Controller ===\r\n");
  uart_puts("Enter delay times in seconds for each light:\r\n");

  uart_get_number("Green duration (sec): ", &green_delay);
  uart_puts("\r\n");

  uart_get_number("Yellow duration (sec): ", &yellow_delay);
  uart_puts("\r\n");

  uart_get_number("Red duration (sec): ", &red_delay);
  uart_puts("\r\n");

  uart_puts("\r\nStarting traffic light FSM...\r\n");

  set_leds(0);
  traffic_light_fsm();

  return 0;
}