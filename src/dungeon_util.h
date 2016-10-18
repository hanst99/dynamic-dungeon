#pragma once

#include <functional>
#include <cstdint>

namespace util {
  class at_exit {
  public:
    using CallbackT = std::function<void(void)>;
    at_exit(CallbackT callback);
    ~at_exit();
  private:
    CallbackT callback;
  };

  constexpr uint32_t MAX_FRAMERATE=60;
  constexpr uint32_t FRAME_INTERVAL=1000/MAX_FRAMERATE + 1;
}
