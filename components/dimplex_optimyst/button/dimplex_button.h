#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

#include "../dimplex_optimyst.h"

namespace esphome {
namespace dimplex_optimyst {

class DimplexButton : public button::Button, public Component {
 public:
  void set_parent(DimplexOptimyst *parent) { this->parent_ = parent; }
  void set_cmd(uint8_t cmd) { this->cmd_ = cmd; }
  void set_duration_ms(uint32_t duration_ms) { this->duration_ms_ = duration_ms; }

 protected:
  void press_action() override;

  DimplexOptimyst *parent_{nullptr};
  uint8_t cmd_{0};
  uint32_t duration_ms_{250};
};

}  // namespace dimplex_optimyst
}  // namespace esphome
