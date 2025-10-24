#ifndef NEO_LED_H
#define NEO_LED_H

typedef enum
{
    NEO_LED_OFF,
    NEO_LED_RED,
    NEO_LED_BLUE,
    NEO_LED_GREEN,
} neo_led_t;

void neo_led_init();
void neo_led_ctrl(neo_led_t led);

#endif // NEO_LED_H