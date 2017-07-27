
#pragma once

#include <stdio.h>
#include "stage.h"

class ssn_ring;
class stage;

class vnf {
 public:
  static constexpr double THRESH = 0.7;
  std::vector<stage*> stages;
  virtual void deploy();
  virtual void debug_dump(FILE* fp);
  virtual void tuneup();
};




