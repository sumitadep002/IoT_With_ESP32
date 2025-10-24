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

void neo_led_init(void)
{
    esp_err_t err;

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

void neo_led_ctrl(neo_led_t led)
{
    uint8_t code[3] = {0}; // GRB format for WS2812

    switch (led)
    {
    case NEO_LED_RED:
        code[0] = 0x00; // G
        code[1] = 0xFF; // R
        code[2] = 0x00; // B
        break;

    case NEO_LED_GREEN:
        code[0] = 0xFF; // G
        code[1] = 0x00; // R
        code[2] = 0x00; // B
        break;

    case NEO_LED_BLUE:
        code[0] = 0x00; // G
        code[1] = 0x00; // R
        code[2] = 0xFF; // B
        break;

    case NEO_LED_OFF:
    default:
        code[0] = 0x00;
        code[1] = 0x00;
        code[2] = 0x00;
        break;
    }

    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, code, sizeof(code), &transmit_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));

    // Latch — hold low for >50 µs
    vTaskDelay(pdMS_TO_TICKS(1));
}