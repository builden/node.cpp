#ifndef __EVENTS_H__
#define __EVENTS_H__
#include "nodecpp-def.h"

namespace nodecpp {

  class EventEmitter {
  public:
    EventEmitter();

    template <typename... Args>
    bool emit(const string& type, Args... args);

    template <typename LMB, typename... Args>
    void _addListener(const string& type, LMB listener, void(LMB::*func)(Args...) const) {
      _events.insert(std::make_pair(type, std::make_shared<Listener<Args...>>(type, function<void(Args...)>(listener))));
    }

    // T是lambda表达式
    template <typename T>
    void addListener(const string& type, T listener) {
      _addListener(type, listener, &T::operator());
    }

    template <typename... Args>
    void once(const string& type, function<void(Args...)> listener);
    void once(const string& type, function<void()> listener);

    template <typename T>
    void on(const string& type, T listener) {
      addListener(type, listener);
    }

    void removeListener(const string& type, function<void()> listener);

    void removeAllListeners(const string& type);

    void setMaxListeners(int n);
    int getMaxListeners();
  private:
    int _maxListeners;
    struct ListenerBase {
      ListenerBase() {}
      ListenerBase(const string& _type) : type(_type) {}
      virtual ~ListenerBase(){};
      string type;
    };

    template <typename... Args>
    struct Listener : public ListenerBase
    {
      Listener() {}

      Listener(const string& type, function<void(Args...)> _listener)
        : ListenerBase(type), listener(_listener) {}

      function<void(Args...)> listener;
    };
    std::multimap<string, shared_ptr<ListenerBase>> _events;
  };

  template <typename... Args>
  bool nodecpp::EventEmitter::emit(const string& type, Args... args) {
    std::list<std::shared_ptr<Listener<Args...>>> handlers;
    auto range = _events.equal_range(type);
    handlers.resize(std::distance(range.first, range.second));
    std::transform(range.first, range.second, handlers.begin(), [](std::pair<const string&, shared_ptr<ListenerBase>> p) {
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

}

#endif // !__EVENTS_H__
