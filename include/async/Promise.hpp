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

    /// Polls the promise to check if it is resolved
    ///
    /// If the promise is resolved, the callback is called
    ///
    /// @return The poll result, either PENDING or READY
    Poll poll() override
    {
        std::optional<T> value = _task();

        if (value.has_value()) {
            _callback(std::move(value.value()));
            return Poll::READY;
        }
        return Poll::PENDING;
    }

    /// Registers a callback to be called when the promise is resolved
    ///
    /// If the promise can be resolved immediately, the callback is called
    /// immediately. Otherwise, the promise is added to the poller
    ///
    /// @param callback The callback to call
    /// @return The promise
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
