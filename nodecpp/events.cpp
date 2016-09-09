#include "events.h"

namespace nodecpp {
  const int defaultMaxListeners = 10;
  EventEmitter::EventEmitter() {
    _maxListeners = defaultMaxListeners;
  }

  void EventEmitter::setMaxListeners(int n) {
    _maxListeners = n;
  }

  int EventEmitter::getMaxListeners() {
    return _maxListeners;
  }

  void EventEmitter::removeAllListeners(const string& type) {
    auto i = std::find_if(_events.begin(), _events.end(), [&](std::pair<const string&, shared_ptr<ListenerBase>> p) {
      return p.second->type == type;
    });
    if (i != _events.end()) {
      _events.erase(i);
    }
    else {
      throw std::invalid_argument("EventEmitter::remove_listener: Invalid listener id.");
    }
  }

}
