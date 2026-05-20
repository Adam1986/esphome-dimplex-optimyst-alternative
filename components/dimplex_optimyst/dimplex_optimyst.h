#pragma once

#include "esphome/core/component.h"

#ifdef USE_ESP32
#include <esp_gap_ble_api.h>
#endif

namespace esphome {
namespace dimplex_optimyst {

class DimplexOptimyst : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  /// Broadcast the given command byte as a BLE non-connectable advertisement
  /// for the given duration (ms). Non-blocking — the stop is scheduled.
  void send_command(uint8_t cmd, uint32_t duration_ms);

 protected:
  void start_advertising_(uint8_t cmd);
  void stop_advertising_();

  uint8_t raw_adv_[15];
  bool advertising_{false};
};

}  // namespace dimplex_optimyst
}  // namespace esphome
