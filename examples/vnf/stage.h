

#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include "stageio.h"



class func {
 public:
  virtual void poll_exe() = 0;
  virtual void stop() = 0;
};

class stage {
 protected:
  size_t mux_;
 public:
  const std::string name;
  std::vector<func*> funcs;
  stage(const char* n) : name(n), mux_(0) {}
  virtual ~stage() {}
  virtual void inc();
  virtual void dec();
  virtual size_t mux() const;
  virtual size_t throughput_pps() const = 0;
  virtual func* allocate() = 0;
};


