
#pragma once

class stageio_rx;
class stageio_tx;

class func {
 protected:
  bool run;
 public:
  std::vector<stageio_rx*>& rx;
  std::vector<stageio_tx*>& tx;

  func(std::vector<stageio_rx*>& _rx,
       std::vector<stageio_tx*>& _tx) : rx(_rx), tx(_tx) {}
  virtual void poll_exe() = 0;
  virtual void stop() = 0;
};


