#include <utility>
#include <exception>
#include <vector>
#include <functional>
#include "Future.h"

#ifndef _PROMISE_H
#define _PROMISE_H

template <class T>
class Promise
{
  enum State
  {
    PENDING,
    FULFILLED,
    REJECTED
  };

  State state = State::PENDING;
  T value;
  std::exception error;
  std::vector<std::function<void(T)>> resultHandlers;
  std::vector<std::function<void(std::exception)>> exceptionHandlers;

public:
  Promise() {}

  Promise *init(std::function<void(Promise)> callback)
  {
    callback(this);
    return this;
  }

  void fulfill(T result)
  {
    if (state != State::PENDING)
      throw std::runtime_error("Promise already completed.");

    state = State::FULFILLED;
    value = result;

    for (auto handler : resultHandlers)
    {
      handler(result);
    }
  }

  void reject(std::exception error)
  {
    if (state != State::PENDING)
      throw std::runtime_error("Promise already completed.");

    state = State::REJECTED;
    value = error;

    for (auto handler : exceptionHandlers)
    {
      handler(error);
    }
  }

  void then(std::function<void(T)> handler)
  {
    if (state == State::REJECTED)
      return;

    if (state == State::FULFILLED)
    {
      handler(value);
    }
    else
    {
      resultHandlers.push_back(handler);
    }
  }

  template <class U>
  Promise<U> *then(std::function<Promise<U> *(T)> handler)
  {
    if (state == State::REJECTED) 
    {
      return new Promise<U>();
    }

    if (state == State::FULFILLED)
    {
      return handler(value);
    }
    else
    {
      Promise<U> *chainPromise = new Promise<U>();
      resultHandlers.push_back([&](T r) {
        Promise<U> *nextPromise = handler(r);
        nextPromise->then([&](U u) {
          chainPromise->fulfill(u);
        });
      });
      return chainPromise;
    }
  }
};

#endif
