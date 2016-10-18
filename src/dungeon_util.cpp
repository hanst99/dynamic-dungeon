#include "dungeon_util.h"

util::at_exit::at_exit(at_exit::CallbackT callback) : callback(callback) {}
util::at_exit::~at_exit() {
  callback();
}
