#include "dimplex_optimyst.h"

#ifdef USE_ESP32

#include "esphome/core/application.h"
#include "esphome/core/log.h"

#include <cstring>

namespace esphome {
namespace dimplex_optimyst {

static const char *const TAG = "dimplex_optimyst";

// Manufacturer-specific advertising frame as captured from a physical
// Dimplex Opti-Myst 6-button remote:
//   Flags AD:                 02 01 04
//   Manufacturer-data AD:     0B FF + company-id-LE 4F 70 + payload
//   Payload:                  74 69 6D 79 73 74 <cmd> 05
//                             ↑ "timyst"          ↑       ↑
//                                                 cmd     trailer
// Combined with the company-ID, the on-air bytes read "Optimyst<cmd>\x08".
static const uint8_t DIMPLEX_TEMPLATE[15] = {
    0x02, 0x01, 0x04,
    0x0B, 0xFF, 0x4F, 0x70,
    0x74, 0x69, 0x6D, 0x79, 0x73, 0x74,
    0x00,  // <-- command byte goes here (index 13)
    0x05,
};
static constexpr size_t CMD_INDEX = 13;
static constexpr size_t TEMPLATE_LEN = sizeof(DIMPLEX_TEMPLATE);

void DimplexOptimyst::setup() {
  std::memcpy(this->raw_adv_, DIMPLEX_TEMPLATE, TEMPLATE_LEN);
}

void DimplexOptimyst::dump_config() {
  ESP_LOGCONFIG(TAG, "Dimplex Opti-Myst:");
  ESP_LOGCONFIG(TAG, "  Address type: PUBLIC (ESP32's built-in MAC)");
}

float DimplexOptimyst::get_setup_priority() const {
  // After BLE stack initialisation.
  return setup_priority::AFTER_BLUETOOTH;
}

void DimplexOptimyst::send_command(uint8_t cmd, uint32_t duration_ms) {
  if (this->advertising_) {
    ESP_LOGW(TAG, "send_command(0x%02X) ignored: already advertising", cmd);
    return;
  }
  this->start_advertising_(cmd);
  this->set_timeout("dimplex_stop", duration_ms,
                    [this]() { this->stop_advertising_(); });
}

void DimplexOptimyst::start_advertising_(uint8_t cmd) {
  this->raw_adv_[CMD_INDEX] = cmd;

  esp_err_t err = esp_ble_gap_config_adv_data_raw(this->raw_adv_, TEMPLATE_LEN);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "config_adv_data_raw failed: 0x%X", err);
    return;
  }

  esp_ble_adv_params_t params = {};
  params.adv_int_min = 0x20;
  params.adv_int_max = 0x40;
  params.adv_type = ADV_TYPE_NONCONN_IND;
  params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  params.channel_map = ADV_CHNL_ALL;
  params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

  err = esp_ble_gap_start_advertising(&params);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "start_advertising failed: 0x%X", err);
    return;
  }

  this->advertising_ = true;
  ESP_LOGI(TAG, "broadcasting cmd 0x%02X", cmd);
}

void DimplexOptimyst::stop_advertising_() {
  esp_err_t err = esp_ble_gap_stop_advertising();
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "stop_advertising failed: 0x%X", err);
  }
  this->advertising_ = false;
  ESP_LOGD(TAG, "stopped");
}

}  // namespace dimplex_optimyst
}  // namespace esphome

#endif  // USE_ESP32
