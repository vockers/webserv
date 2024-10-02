#pragma once

#include <functional>
#include <optional>

#include "async/Poller.hpp"

namespace webserv::async
{
template <typename T>
class Promise
{
public:
    using TaskFn = std::function<std::optional<T>()>;
    using RejectFn = std::function<void()>;

    Promise(TaskFn task, int fd, Event::Type type)
        : _task(task), _reject([this, fd, type]() { 
            Poller::instance().add(Event(fd, type, this->poll));})
    {
    }

    Promise<T>& then(std::function<void(T)> callback)
    {
        std::optional<T> value = _task();

        if (value.has_value()) {
            callback(value.value());
        } else {
            _reject();
        }
    }

    void poll()
    {
        std::optional<T> value = _task();

        if (value.has_value()) {
            callback(value.value());
        }
    } 

private:
    TaskFn   _task;
    RejectFn _reject;
};
}
