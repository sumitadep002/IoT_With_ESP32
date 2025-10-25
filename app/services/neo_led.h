#ifndef NEO_LED_H
#define NEO_LED_H

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint16_t delay_ms;
} neo_led_queue_t;

void neo_led_init();
void neo_led_ctrl(uint8_t r, uint8_t g, uint8_t b);
void neo_led_task_init();
void neo_led_queue_send(neo_led_queue_t param);

#endif // NEO_LED_H