#ifndef NEO_LED_H
#define NEO_LED_H

typedef enum
{
    NEO_LED_OFF,
    NEO_LED_RED,
    NEO_LED_BLUE,
    NEO_LED_GREEN,
    NEO_LED_ORANGE,
} neo_led_t;

typedef struct
{
    neo_led_t color;
    uint16_t delay_ms;
    bool latch;
} neo_led_queue_t;

void neo_led_init();
void neo_led_ctrl(neo_led_t led);
void neo_led_task_init();
void neo_led_queue_send(neo_led_queue_t param);

#endif // NEO_LED_H