// pedal_display.h — LovyanGFX config for MSP3526 (3.5" IPS, ST7796S + FT6336 touch)
// on ESP32-S3, matching the pedal pin map:
//   SCK=15  MOSI=16  MISO=17  LCD_CS=18  DC=8  LCD_RST=9  LED(BL)=10
//   Touch: SDA=1  SCL=2  INT=47  CTP_RST=21
// Reserved by the pedal (do not reuse): 4,5,6,7 (analog/controls) 12,13,14 (I2S)
//
// Requires: LovyanGFX library (Library Manager -> "LovyanGFX")

#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX_Pedal : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796  _panel;
  lgfx::Bus_SPI       _bus;
  lgfx::Light_PWM     _light;
  lgfx::Touch_FT5x06  _touch;   // FT6336 is handled by the FT5x06 driver

public:
  LGFX_Pedal(void) {
    { // SPI bus
      auto cfg = _bus.config();
      cfg.spi_host    = SPI3_HOST;
      cfg.spi_mode    = 0;
      cfg.freq_write  = 40000000;   // 40 MHz max through GPIO matrix
      cfg.freq_read   = 16000000;
      cfg.spi_3wire   = false;
      cfg.use_lock    = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 15;
      cfg.pin_mosi = 16;
      cfg.pin_miso = 17;            // set to -1 if you left SDO unwired
      cfg.pin_dc   = 8;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    { // panel
      auto cfg = _panel.config();
      cfg.pin_cs   = 18;
      cfg.pin_rst  = 9;
      cfg.pin_busy = -1;
      cfg.panel_width   = 320;
      cfg.panel_height  = 480;
      cfg.offset_x      = 0;
      cfg.offset_y      = 0;
      cfg.offset_rotation = 0;
      cfg.readable   = true;        // false if MISO unwired
      cfg.invert     = false;       // colors negative/washed out? -> true
      cfg.rgb_order  = false;       // red and blue swapped? -> true
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;       // true if SD card shares this SPI bus
      _panel.config(cfg);
    }
    { // backlight (PWM dimmable on GPIO10)
      auto cfg = _light.config();
      cfg.pin_bl      = 10;
      cfg.invert      = false;
      cfg.freq        = 12000;
      cfg.pwm_channel = 7;
      _light.config(cfg);
      _panel.setLight(&_light);
    }
    { // capacitive touch (FT6336, I2C)
      auto cfg = _touch.config();
      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda  = 1;
      cfg.pin_scl  = 2;
      cfg.pin_int  = 47;
      cfg.freq     = 400000;
      cfg.x_min = 0; cfg.x_max = 319;
      cfg.y_min = 0; cfg.y_max = 479;
      cfg.offset_rotation = 0;      // touch mirrored vs display? try 0..7
      _touch.config(cfg);
      _panel.setTouch(&_touch);
    }
    setPanel(&_panel);
  }
};

// ---- usage ----------------------------------------------------------------
// LGFX_Pedal lcd;
//
// void displayTask(void*) {                 // UI on core 0; audio stays on core 1
//   pinMode(21, OUTPUT);                    // CTP_RST: release touch reset
//   digitalWrite(21, HIGH);
//   delay(10);
//   lcd.init();
//   lcd.setRotation(1);                     // 1 = landscape, 480x320
//   lcd.setBrightness(180);                 // 0-255
//   lcd.fillScreen(TFT_BLACK);
//   lcd.setTextSize(3);
//   lcd.drawString("ESP32 PEDAL", 20, 20);
//
//   for (;;) {
//     uint16_t x, y;
//     if (lcd.getTouch(&x, &y)) {
//       lcd.fillCircle(x, y, 4, TFT_GREEN);  // touch test: draw where pressed
//     }
//     vTaskDelay(pdMS_TO_TICKS(10));
//   }
// }
//
// // in setup(), after audio init:
// // xTaskCreatePinnedToCore(displayTask, "ui", 8192, nullptr, 1, nullptr, 0);