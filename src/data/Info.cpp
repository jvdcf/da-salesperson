#include "Info.h"
#include "../Utils.h"


Info::Info(uint64_t id) {
  this->id = id;
  this->lat = std::nullopt;
  this->lon = std::nullopt;
}

Info::Info(uint64_t id, double  lat, double lon) {
  this->id = id;
  this->lat = lat;
  this->lon = lon;
}

uint64_t Info::getId() const {
  return this->id;
}

std::optional<double> Info::getLat() const {
  return this->lat;
}

std::optional<double> Info::getLon() const {
  return this->lon;
}

double Info::distance(const Info &other) const {
  return Utils::haversineDistance(
          this->lat.value(), this->lon.value(),
          other.getLat().value(), other.getLon().value()
          );
}

bool Info::operator==(const Info &rhs) const {
  return this->id == rhs.id;
}
