#pragma once

#include <functional>
#include <optional>
#include <iostream>

#include "async/Poller.hpp"

namespace webserv::async
{
class IPromise
{
public:
    virtual ~IPromise() = default;
    virtual Poll poll() { return Poll::READY; }
};

template <typename T>
class Promise : public IPromise
{
public:
    using TaskFn = std::function<std::optional<T>()>;
    using RejectFn = std::function<void()>;
    using CallbackFn = std::function<void(T)>;

    Promise(TaskFn task, int fd, Event::Type type)
        : _task(task), _reject([this, fd, type]() { 
            Poller::instance().add_promise(*this, fd, type);})
    {
    }

    Poll poll() override
    {
        std::optional<T> value = _task();

        std::cout << "Promise::poll: no value" << std::endl;
        if (value.has_value() && _callback) {
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
            std::cout << "Promise::then: no value" << std::endl;
            _callback = callback;
            _reject();
        }

        return *this;
    }

private:
    TaskFn   _task;
    RejectFn _reject;
    CallbackFn _callback;
};
}
