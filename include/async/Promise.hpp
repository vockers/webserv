#pragma once

#include <functional>
#include <optional>

#include "async/Poller.hpp"

namespace webserv::async
{
class IPromise
{
public:
    virtual ~IPromise() = default;
    virtual Poll poll() = 0;
};

template <typename T>
class Promise : public IPromise
{
public:
    using TaskFn     = std::function<std::optional<T>()>;
    using RejectFn   = std::function<void(Promise<T>)>;
    using CallbackFn = std::function<void(T)>;

    Promise(TaskFn task, int fd, Event::Type type)
        : _task(task), _reject([fd, type](Promise<T> promise) {
              auto promise_ptr = std::make_unique<Promise<T>>(promise);
              Poller::instance().add_promise(std::move(promise_ptr), fd, type);
          })
    {
    }

    Promise(TaskFn task)
        : _task(task), _reject([](Promise<T> promise) {
              auto promise_ptr = std::make_unique<Promise<T>>(promise);
              Poller::instance().add_promise(std::move(promise_ptr));
          })
    {
    }

    Poll poll() override
    {
        std::optional<T> value = _task();

        if (value.has_value()) {
            _callback(std::move(value.value()));
            return Poll::READY;
        }
        return Poll::PENDING;
    }

    Promise<T>& then(CallbackFn callback)
    {
        std::optional<T> value = _task();

        if (value.has_value()) {
            callback(std::move(value.value()));
        } else {
            _callback = callback;
            _reject(*this);
        }

        return *this;
    }

private:
    TaskFn     _task;
    RejectFn   _reject;
    CallbackFn _callback;
};
}  // namespace webserv::async
