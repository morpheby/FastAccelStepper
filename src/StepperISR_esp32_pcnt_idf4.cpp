#include "StepperISR.h"
#if defined(SUPPORT_ESP32_PULSE_COUNTER) && (ESP_IDF_VERSION_MAJOR == 4)

#ifndef HAVE_ESP32S3_PULSE_COUNTER
#define PCNT_MODULE_CNT 8
#else
#define PCNT_MODULE_CNT 4
#endif
uint32_t sig_idx[PCNT_MODULE_CNT] = {
	PCNT_SIG_CH0_IN0_IDX, PCNT_SIG_CH0_IN1_IDX,
	PCNT_SIG_CH0_IN2_IDX, PCNT_SIG_CH0_IN3_IDX,
#if PCNT_MODULE_CNT == 8
	PCNT_SIG_CH0_IN4_IDX, PCNT_SIG_CH0_IN5_IDX,
	PCNT_SIG_CH0_IN6_IDX, PCNT_SIG_CH0_IN7_IDX,
#endif
};
uint32_t ctrl_idx[PCNT_MODULE_CNT] = {
	PCNT_CTRL_CH0_IN0_IDX, PCNT_CTRL_CH0_IN1_IDX,
	PCNT_CTRL_CH0_IN2_IDX, PCNT_CTRL_CH0_IN3_IDX,
#if PCNT_MODULE_CNT == 8
	PCNT_CTRL_CH0_IN4_IDX, PCNT_CTRL_CH0_IN5_IDX,
	PCNT_CTRL_CH0_IN6_IDX, PCNT_CTRL_CH0_IN7_IDX
#endif
};

bool _esp32_attachToPulseCounter(uint8_t pcnt_unit, FastAccelStepper *stepper,
                                 int16_t low_value, int16_t high_value) {
  // TODO: Check if free pulse counter
  if (pcnt_unit >= PCNT_MODULE_CNT) {
    // fail
    return false;
  }
  pcnt_config_t cfg;
  uint8_t dir_pin = stepper->getDirectionPin();
  cfg.pulse_gpio_num = stepper->getStepPin();
  if (dir_pin == PIN_UNDEFINED) {
    cfg.ctrl_gpio_num = PCNT_PIN_NOT_USED;
    cfg.hctrl_mode = PCNT_MODE_KEEP;
    cfg.lctrl_mode = PCNT_MODE_KEEP;
  } else {
    cfg.ctrl_gpio_num = dir_pin;
    if (stepper->directionPinHighCountsUp()) {
      cfg.lctrl_mode = PCNT_MODE_REVERSE;
      cfg.hctrl_mode = PCNT_MODE_KEEP;
    } else {
      cfg.lctrl_mode = PCNT_MODE_KEEP;
      cfg.hctrl_mode = PCNT_MODE_REVERSE;
    }
  }
  cfg.pos_mode = PCNT_COUNT_INC;  // increment on rising edge
  cfg.neg_mode = PCNT_COUNT_DIS;  // ignore falling edge
  cfg.counter_h_lim = high_value;
  cfg.counter_l_lim = low_value;
  cfg.unit = (pcnt_unit_t)pcnt_unit;
  cfg.channel = PCNT_CHANNEL_0;
  pcnt_unit_config(&cfg);

#ifndef HAVE_ESP32S3_PULSE_COUNTER
  PCNT.conf_unit[cfg.unit].conf0.thr_h_lim_en = 0;
  PCNT.conf_unit[cfg.unit].conf0.thr_l_lim_en = 0;
#else
  PCNT.conf_unit[cfg.unit].conf0.thr_h_lim_en_un = 0;
  PCNT.conf_unit[cfg.unit].conf0.thr_l_lim_en_un = 0;
#endif

  stepper->detachFromPin();
  stepper->reAttachToPin();
  gpio_iomux_in(stepper->getStepPin(), sig_idx[pcnt_unit]);
  if (dir_pin != PIN_UNDEFINED) {
    gpio_matrix_out(stepper->getDirectionPin(), 0x100, false, false);
    gpio_iomux_in(stepper->getDirectionPin(), ctrl_idx[pcnt_unit]);
    pinMode(stepper->getDirectionPin(), OUTPUT);
  }

  pcnt_counter_clear(cfg.unit);
  pcnt_counter_resume(cfg.unit);
  return true;
}
void _esp32_clearPulseCounter(uint8_t pcnt_unit) {
  pcnt_counter_clear((pcnt_unit_t)pcnt_unit);
}
int16_t _esp32_readPulseCounter(uint8_t pcnt_unit) {
  // Serial.println(' ');
  // Serial.println(PCNT.cnt_unit[PCNT_UNIT_0].cnt_val);
  // Serial.println(PCNT.conf_unit[PCNT_UNIT_0].conf2.cnt_h_lim);
#ifndef HAVE_ESP32S3_PULSE_COUNTER
  return PCNT.cnt_unit[(pcnt_unit_t)pcnt_unit].cnt_val;
#else
  return PCNT.cnt_unit[(pcnt_unit_t)pcnt_unit].pulse_cnt_un;
#endif
}
#endif

