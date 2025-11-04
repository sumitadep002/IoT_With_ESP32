#include "freertos/FreeRTOS.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/gpio_num.h"
#include "defines.h"
#include "neo_led.h"

#define TAG_NEO_LED "NEO_LED"

// WS2812 timing for 10 MHz RMT clock (0.1 µs per tick)
#define T0H 4 // 0.4 µs high
#define T0L 8 // 0.8 µs low
#define T1H 8 // 0.8 µs high
#define T1L 4 // 0.4 µs low

static rmt_tx_channel_config_t tx_chan_config = {0};
static rmt_channel_handle_t tx_chan = NULL;
static rmt_transmit_config_t transmit_config = {0};
static rmt_bytes_encoder_config_t encoder_config = {0};
static rmt_encoder_handle_t encoder = NULL;

static QueueHandle_t neo_led_queue = NULL;

static void neo_led_task(void *param);

void neo_led_init(void)
{

    // --- Configure RMT TX channel ---
    tx_chan_config.clk_src = RMT_CLK_SRC_DEFAULT;
    tx_chan_config.gpio_num = NEO_LED_GPIO;
    tx_chan_config.mem_block_symbols = 64;
    tx_chan_config.resolution_hz = 10 * 1000 * 1000; // 10 MHz
    tx_chan_config.trans_queue_depth = 4;
    tx_chan_config.flags.invert_out = false;
    tx_chan_config.flags.with_dma = false;

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    // --- Configure RMT encoder ---
    rmt_symbol_word_t bit0 = {
        .level0 = 1, .duration0 = T0H, .level1 = 0, .duration1 = T0L};
    rmt_symbol_word_t bit1 = {
        .level0 = 1, .duration0 = T1H, .level1 = 0, .duration1 = T1L};

    encoder_config.bit0 = bit0;
    encoder_config.bit1 = bit1;
    encoder_config.flags.msb_first = 1; // WS2812 expects MSB first
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&encoder_config, &encoder));

    // --- Transmission settings ---
    transmit_config.loop_count = 0;
    transmit_config.flags.eot_level = 0; // line low after transmission
    transmit_config.flags.queue_nonblocking = false;

    ESP_LOGI(TAG_NEO_LED, "NeoPixel LED initialized on GPIO %d", NEO_LED_GPIO);
}

void neo_led_ctrl(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t code[3];

    code[0] = g; // WS2812 expects GRB order
    code[1] = r;
    code[2] = b;

    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, code, sizeof(code), &transmit_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));

    // Latch — hold low for >50 µs
    DELAY_MS(1);
}

void neo_led_task_init(void)
{
    neo_led_init();
    neo_led_queue = xQueueCreate(10, sizeof(neo_led_queue_t));
    if (neo_led_queue == NULL)
    {
        ESP_LOGE("LED", "Queue creation failed");
        return;
    }

    BaseType_t status = xTaskCreate(neo_led_task, "neo_led_task", 4096, NULL, 5, NULL);
    if (status != pdPASS)
    {
        ESP_LOGE("LED", "Failed to create LED task");
    }
}

void neo_led_queue_send(neo_led_queue_t param)
{
    if (neo_led_queue != NULL)
    {
        xQueueSend(neo_led_queue, &param, portMAX_DELAY);
    }
}

void neo_led_task(void *param)
{
    neo_led_queue_t led_comp;

    while (1)
    {
        if (xQueueReceive(neo_led_queue, &led_comp, portMAX_DELAY) == pdPASS)
        {
            neo_led_ctrl(led_comp.r, led_comp.g, led_comp.b);

            if (led_comp.delay_ms != 0)
            {
                DELAY_MS(led_comp.delay_ms);
                neo_led_ctrl(0, 0, 0);
                DELAY_MS(led_comp.delay_ms);
            }
        }
    }
}