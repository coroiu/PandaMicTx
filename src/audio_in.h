#ifndef _AUDIO_IN_H
#define _AUDIO_IN_H

#include "driver/i2s.h"
#include "freertos/queue.h"

// I2S
// BCLK: 17
// LRCL: 16
// DOUT: 19

#define I2S_PORT I2S_NUM_0

static const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
    .sample_rate = 44100,                              // 44.1KHz
    // .sample_rate = 48000,                              // 44.1KHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
    // .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,       // although the SEL config should be left, it seems to transmit on right
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // although the SEL config should be left, it seems to transmit on right
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3, // Interrupt level 1
    // .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
    .dma_buf_count = 3, // number of buffers
    .dma_buf_len = 128  // samples per buffer
};

static const i2s_pin_config_t pin_config = {
    .bck_io_num = 17,   // BCKL
    .ws_io_num = 16,    // LRCL
    .data_out_num = -1, // not used (only for speakers)
    .data_in_num = 19   // DOUT
};

void audio_init()
{
  ESP_ERROR_CHECK(i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL));
  ESP_ERROR_CHECK(i2s_set_pin(I2S_PORT, &pin_config));
}

void audio_deinit()
{
  ESP_ERROR_CHECK(i2s_driver_uninstall(I2S_PORT));
}

void audio_start()
{
  ESP_ERROR_CHECK(i2s_start(I2S_PORT));
}

void audio_stop()
{
  ESP_ERROR_CHECK(i2s_stop(I2S_PORT));
}

#endif
