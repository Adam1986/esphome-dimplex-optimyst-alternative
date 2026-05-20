#include "dimplex_button.h"

namespace esphome {
namespace dimplex_optimyst {

void DimplexButton::press_action() {
  if (this->parent_ == nullptr) return;
  this->parent_->send_command(this->cmd_, this->duration_ms_);
}

}  // namespace dimplex_optimyst
}  // namespace esphome
