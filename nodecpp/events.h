#ifndef __EVENTS_H__
#define __EVENTS_H__
#include "nodecpp-def.h"

namespace nodecpp {

  class EventEmitter {
  public:
    EventEmitter();

    template <typename... Args>
    bool emit(const std::string& type, Args... args);

    template <typename... Args>
    void addListener(const std::string& type, std::function<void(Args...)> listener);
    void addListener(const std::string& type, std::function<void()> listener);

    template <typename... Args>
    void once(const std::string& type, std::function<void(Args...)> listener);
    void once(const std::string& type, std::function<void()> listener);

    template <typename... Args>
    void on(const std::string& type, std::function<void(Args...)> listener);
    void on(const std::string& type, std::function<void()> listener);

    void removeListener(const std::string& type, std::function<void()> listener);

    void removeAllListeners(const std::string& type);

    void setMaxListeners(int n);
    int getMaxListeners();
  private:
    int _maxListeners;
    struct ListenerBase {
      ListenerBase() {}
      ListenerBase(const std::string& _type) : type(_type) {}
      virtual ~ListenerBase(){};
      std::string type;
    };

    template <typename... Args>
    struct Listener : public ListenerBase
    {
      Listener() {}

      Listener(const std::string& type, std::function<void(Args...)> _listener)
        : ListenerBase(type), listener(_listener) {}

      std::function<void(Args...)> listener;
    };
    std::multimap<std::string, std::shared_ptr<ListenerBase>> _events;
  };

  template <typename... Args>
  void nodecpp::EventEmitter::on(const std::string& type, std::function<void(Args...)> listener) {
    addListener(type, listener);
  }

  template <typename... Args>
  bool nodecpp::EventEmitter::emit(const std::string& type, Args... args) {
    std::list<std::shared_ptr<Listener<Args...>>> handlers;
    auto range = _events.equal_range(type);
    handlers.resize(std::distance(range.first, range.second));
    std::transform(range.first, range.second, handlers.begin(), [](std::pair<const std::string&, std::shared_ptr<ListenerBase>> p) {
      auto l = std::dynamic_pointer_cast<Listener<Args...>>(p.second);
      if (l) {
        return l;
      }
      else {
        throw std::logic_error("EventEmitter::emit: Invalid event signature.");
      }
    });

    for (auto& h : handlers) {
      h->listener(args...);
    }
    return true;
  }

  template <typename... Args>
  void nodecpp::EventEmitter::addListener(const std::string& type, std::function<void(Args...)> listener) {
    if (!listener) throw std::invalid_argument("EventEmitter::addListener: No callbak provided.");

    _events.insert(std::make_pair(type, std::make_shared<Listener<Args...>>(type, listener)));
  }

}

#endif // !__EVENTS_H__
