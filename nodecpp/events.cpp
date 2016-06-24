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

  void EventEmitter::addListener(const std::string& type, std::function<void()> listener) {
    if (!listener) throw std::invalid_argument("EventEmitter::addListener: No callbak provided.");

    _events.insert(std::make_pair(type, std::make_shared<Listener<>>(type, listener)));
  }

  void EventEmitter::removeAllListeners(const std::string& type) {
    auto i = std::find_if(_events.begin(), _events.end(), [&](std::pair<const std::string&, std::shared_ptr<ListenerBase>> p) {
      return p.second->type == type;
    });
    if (i != _events.end()) {
      _events.erase(i);
    }
    else {
      throw std::invalid_argument("EventEmitter::remove_listener: Invalid listener id.");
    }
  }

  void EventEmitter::on(const std::string& type, std::function<void()> listener) {
    addListener(type, listener);
  }

}
