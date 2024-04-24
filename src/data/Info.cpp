#include "Info.h"

bool Info::isActive() const {
  return is_active;
}

void Info::enable() {
  this->is_active = true;
}

void Info::disable() {
  this->is_active = false;
}
