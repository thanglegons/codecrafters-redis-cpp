#include "ReplicationInfo.h"

std::string ReplicationInfo::returnRawInfo() const {
  return "role:" + (is_master ? std::string("master") : std::string("slave"));
}