// pcm1808_pedal_ui.ino — guitar pedal with touch display UI
// PCM1808 ADC + PCM5102A DAC + MSP3526 LCD (ST7796 + FT6336 touch)
//
// Sketch folder must also contain: pedal_display.h  (LovyanGFX config, unchanged)
// Library: LovyanGFX. Board: "ESP32S3 Dev Module", USB CDC On Boot: Enabled.
//
// Audio pins: GPIO12 BCK (shared), GPIO14 LRC+LCK (shared), GPIO42 MCLK->ADC SCK,
//             GPIO41 <- ADC OUT, GPIO13 -> DAC DIN
// Display:    per pedal_display.h (15,16,17,18,8,9,10 + touch 1,2,21,47)
// Controls:   GPIO5 footswitch, GPIO6 LED, GPIO7 pot
//             + touch buttons + serial: 'e' = on/off, 'n' = next effect
//
// UI runs on core 0, audio on core 1 - screen redraws never interrupt sound.

#include "driver/i2s_std.h"
#include "pedal_display.h"

#define USE_POT  1   // 0 = no pot wired: knob stays at a fixed value

#define SAMPLE_RATE 32000
#define FRAME_LEN   256
#define PIN_FOOT    5
#define PIN_LED     6
#define PIN_POT     7
#define PIN_ENC_A   4      // rotary encoder CLK
#define PIN_ENC_B   38     // rotary encoder DT
#define PIN_ENC_SW  39     // rotary encoder push button

volatile int encDelta = 0;
void IRAM_ATTR encISR() {
  static uint32_t tLast = 0;
  uint32_t now = micros();
  if (now - tLast < 2000) return;        // 2 ms glitch filter
  tLast = now;
  encDelta += digitalRead(PIN_ENC_B) ? -1 : 1;
}
#define IN_GAIN     6.0f
#define OUT_LEVEL   0.5f

enum Effect : int { FX_DIST = 0, FX_CHORUS, FX_FUZZ, FX_TREM, FX_DELAY, FX_CRUSH, FX_COUNT };
const char* FX_NAMES[FX_COUNT] = { "DISTORTION", "CHORUS", "FUZZ", "TREMOLO", "DELAY", "CRUSHER" };
volatile bool  effectOn  = true;
volatile int   effectSel = FX_DIST;
float knob = 0.5f;

i2s_chan_handle_t tx = nullptr, rx = nullptr;
static int32_t inBuf[FRAME_LEN * 2], outBuf[FRAME_LEN * 2];

static float   tremPhase = 0.0f;
static int16_t dbuf[16000];
static int     dWrite = 0;
static float   crushHold = 0.0f;
static int     crushCnt = 0;

// ---------------- audio ----------------
void setupI2S() {
  i2s_chan_config_t ch = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  ESP_ERROR_CHECK(i2s_new_channel(&ch, &tx, &rx));
  i2s_std_config_t std = {
    .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT,
                                                    I2S_SLOT_MODE_STEREO),
    .gpio_cfg = { .mclk = GPIO_NUM_42, .bclk = GPIO_NUM_12,
                  .ws = GPIO_NUM_14, .dout = GPIO_NUM_13,
                  .din = GPIO_NUM_41,
                  .invert_flags = { false, false, false } },
  };
  ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx, &std));
  ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx, &std));
  ESP_ERROR_CHECK(i2s_channel_enable(tx));
  ESP_ERROR_CHECK(i2s_channel_enable(rx));
}

static inline float processSample(float x) {
  float k = knob;
  switch (effectSel) {
    case FX_DIST: {
      float drive = 1.0f + k * 40.0f;
      float y = x * drive;
      y = y / (1.0f + fabsf(y));
      static float lp = 0.0f;
      lp += 0.55f * (y - lp);
      return lp * 1.4f;
    }
    case FX_CHORUS: {
      static int16_t cb[1056];
      static int cw = 0;
      static float ph = 0.0f;
      cb[cw] = (int16_t)(x * 32767.0f);
      float rate = 0.3f + k * 2.7f;
      ph += 2.0f * PI * rate / SAMPLE_RATE;
      if (ph > 2.0f * PI) ph -= 2.0f * PI;
      float rp = (float)cw - (384.0f + 160.0f * sinf(ph));
      while (rp < 0) rp += 1056.0f;
      int i0 = (int)rp; float fr = rp - i0;
      int i1 = i0 + 1; if (i1 >= 1056) i1 = 0;
      float wet = (cb[i0] * (1.0f - fr) + cb[i1] * fr) / 32767.0f;
      if (++cw >= 1056) cw = 0;
      return 0.65f * x + 0.65f * wet;
    }
    case FX_FUZZ: {
      float d = 2.0f + k * 28.0f;
      x *= d;
      if (x > 1) x = 1; if (x < -1) x = -1;
      return x - (x * x * x) / 3.0f;
    }
    case FX_TREM: {
      float rate = 1.0f + k * 9.0f;
      tremPhase += 2.0f * PI * rate / SAMPLE_RATE;
      if (tremPhase > 2.0f * PI) tremPhase -= 2.0f * PI;
      return x * (0.35f + 0.65f * (0.5f + 0.5f * sinf(tremPhase)));
    }
    case FX_DELAY: {
      int dLen = 1600 + (int)(k * 14000);
      int rd = dWrite - dLen; if (rd < 0) rd += 16000;
      float echo = dbuf[rd] / 32767.0f;
      float out = x + 0.5f * echo;
      float fb  = x + 0.35f * echo;
      if (fb > 1) fb = 1; if (fb < -1) fb = -1;
      dbuf[dWrite] = (int16_t)(fb * 32767.0f);
      if (++dWrite >= 16000) dWrite = 0;
      return out * 0.8f;
    }
    case FX_CRUSH: {
      int step = 1 + (int)(k * 7);
      float levels = 256.0f - k * 224.0f;
      if (++crushCnt >= step) { crushCnt = 0; crushHold = x; }
      return roundf(crushHold * levels) / levels;
    }
  }
  return x;
}

void checkControls() {
  static int last = HIGH; static uint32_t tDown = 0; static bool held = false;
  static uint32_t tPot = 0;
  uint32_t now = millis();

  if (Serial.available()) {                       // keyboard fallback
    char c = Serial.read();
    if (c == 'e') effectOn = !effectOn;
    if (c == 'n') effectSel = (effectSel + 1) % FX_COUNT;
  }

  if (encDelta != 0) {                            // rotary: cycle effects
    int d = encDelta; encDelta = 0;
    int sel = ((int)effectSel + d) % FX_COUNT;
    while (sel < 0) sel += FX_COUNT;
    effectSel = sel;
    Serial.printf("effect: %s\n", FX_NAMES[effectSel]);
  }
  static int swLast = HIGH; static uint32_t tSw = 0;
  int sw = digitalRead(PIN_ENC_SW);               // encoder push = on/off
  if (sw == LOW && swLast == HIGH && now - tSw > 250) {
    tSw = now;
    effectOn = !effectOn;
  }
  swLast = sw;

  int s = digitalRead(PIN_FOOT);
  if (s == LOW && last == HIGH) { tDown = now; held = false; }
  if (s == LOW && !held && now - tDown > 1000) {
    held = true;
    effectSel = (effectSel + 1) % FX_COUNT;
  }
  if (s == HIGH && last == LOW && !held && now - tDown > 30)
    effectOn = !effectOn;
  last = s;
  digitalWrite(PIN_LED, effectOn);

#if USE_POT
  if (now - tPot > 50) {
    tPot = now;
    knob += 0.2f * ((analogRead(PIN_POT) / 4095.0f) - knob);
  }
#else
  (void)tPot;   // no pot: knob stays fixed
  knob = 0.35f;
#endif
}

// ---------------- display UI (core 0) ----------------
LGFX_Pedal lcd;
struct Btn { int x, y, w, h; };
const Btn BTN_PWR = { 350, 8, 122, 88 };
const Btn BTN_FX[FX_COUNT] = {
  {8,170,150,60}, {166,170,150,60}, {324,170,150,60},
  {8,240,150,60}, {166,240,150,60}, {324,240,150,60} };

void drawUI(bool full) {
  static int lastSel = -1, lastOn = -1, lastBar = -1;
  if (full) {
    lcd.fillScreen(TFT_BLACK);
    lastSel = lastOn = lastBar = -1;
    lcd.setTextSize(2);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.drawString("ESP32 PEDAL", 8, 10);
  }
  if ((int)effectOn != lastOn) {
    lastOn = effectOn;
    lcd.fillRoundRect(BTN_PWR.x, BTN_PWR.y, BTN_PWR.w, BTN_PWR.h, 10,
                      effectOn ? TFT_GREEN : 0x4208);
    lcd.setTextSize(3);
    lcd.setTextColor(TFT_BLACK, effectOn ? TFT_GREEN : 0x4208);
    lcd.drawString(effectOn ? "ON" : "OFF", BTN_PWR.x + 34, BTN_PWR.y + 32);
  }
  if (effectSel != lastSel) {
    lastSel = effectSel;
    lcd.fillRect(0, 40, 344, 40, TFT_BLACK);
    lcd.setTextSize(3);
    lcd.setTextColor(TFT_CYAN, TFT_BLACK);
    lcd.drawString(FX_NAMES[effectSel], 8, 48);
    for (int i = 0; i < FX_COUNT; i++) {
      bool sel = (i == effectSel);
      lcd.fillRoundRect(BTN_FX[i].x, BTN_FX[i].y, BTN_FX[i].w, BTN_FX[i].h, 8,
                        sel ? TFT_CYAN : 0x2104);
      lcd.setTextSize(2);
      lcd.setTextColor(sel ? TFT_BLACK : TFT_WHITE, sel ? TFT_CYAN : 0x2104);
      lcd.drawString(FX_NAMES[i], BTN_FX[i].x + 8, BTN_FX[i].y + 22);
    }
  }
  int bar = (int)(knob * 300);
  if (abs(bar - lastBar) > 5) {
    lastBar = bar;
    lcd.fillRect(8, 106, 300, 20, 0x2104);
    lcd.fillRect(8, 106, bar, 20, TFT_ORANGE);
    lcd.drawRect(8, 106, 300, 20, TFT_WHITE);
    lcd.setTextSize(2);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.drawString("KNOB", 316, 108);
  }
}

static inline bool hitBox(int bx, int by, int bw, int bh, int x, int y) {
  return x > bx && x < bx + bw && y > by && y < by + bh;
}

void uiTask(void*) {
  pinMode(21, OUTPUT);                  // CTP_RST pulse
  digitalWrite(21, LOW);  vTaskDelay(pdMS_TO_TICKS(10));
  digitalWrite(21, HIGH); vTaskDelay(pdMS_TO_TICKS(50));
  lcd.init();
  lcd.setRotation(1);                   // 480x320 landscape
  lcd.setBrightness(200);
  drawUI(true);
  uint32_t lastTouch = 0;
  for (;;) {
    uint16_t x, y;
    if (lcd.getTouch(&x, &y) && millis() - lastTouch > 250) {
      lastTouch = millis();
      if (hitBox(BTN_PWR.x, BTN_PWR.y, BTN_PWR.w, BTN_PWR.h, x, y))
        effectOn = !effectOn;
      for (int i = 0; i < FX_COUNT; i++)
        if (hitBox(BTN_FX[i].x, BTN_FX[i].y, BTN_FX[i].w, BTN_FX[i].h, x, y)) {
          effectSel = i; effectOn = true;
        }
    }
    drawUI(false);
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}

// ---------------- main ----------------
void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PIN_FOOT, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_A), encISR, FALLING);
  analogReadResolution(12);
  setupI2S();
  xTaskCreatePinnedToCore(uiTask, "ui", 8192, nullptr, 1, nullptr, 0);
  Serial.println("pedal+display running ('e' toggle, 'n' next effect)");
}

void loop() {                           // audio, core 1
  checkControls();

  size_t got = 0;
  if (i2s_channel_read(rx, inBuf, sizeof(inBuf), &got, 100) != ESP_OK) return;
  int frames = got / 8;

  for (int i = 0; i < frames; i++) {
    int32_t raw = inBuf[2 * i];
    float x = (float)(raw >> 8) / 8388608.0f * IN_GAIN;
    if (x > 1) x = 1; if (x < -1) x = -1;

    if (effectOn) x = processSample(x);

    x *= OUT_LEVEL;
    int32_t s = (int32_t)(x * 8388607.0f) << 8;
    outBuf[2 * i]     = s;
    outBuf[2 * i + 1] = s;
  }
  size_t w;
  i2s_channel_write(tx, outBuf, frames * 8, &w, 100);
}
