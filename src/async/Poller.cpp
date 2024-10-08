#include "async/Poller.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>

#include "async/Promise.hpp"

namespace webserv::async
{
Poller::Poller()
{
    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
    if (fcntl(_epoll_fd, F_SETFD, FD_CLOEXEC) == -1) {
        throw std::runtime_error("Failed to set FD_CLOEXEC on epoll instance");
    }
}

Poller::~Poller()
{
    close(_epoll_fd);
}

void Poller::poll()
{
    epoll_event events[10];
    int         num_events = epoll_wait(_epoll_fd, events, 10, -1);
    if (num_events == -1) {
        throw std::runtime_error("Failed to wait for epoll events");
    }

    for (int i = 0; i < num_events; i++) {
        Event* event = static_cast<Event*>(events[i].data.ptr);
        if (events[i].events & EPOLLIN) {
            Poll poll = event->poll();
            if (poll == Poll::READY) {
                epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, event->get_fd(), nullptr);
                _promises.erase(event->get_fd());
                _events.erase(event->get_fd());
            }
        } else if (events[i].events & EPOLLOUT) {
            event->poll();
        }
    }
}

void Poller::add_promise(std::unique_ptr<IPromise> promise, int fd, Event::Type type)
{
    if (_promises.find(fd) != _promises.end()) {
        return;
    }

    this->add_event(Event(fd, type, std::bind(&IPromise::poll, promise.get())));

    _promises[fd] = std::move(promise);
}

void Poller::add_event(Event event)
{
    if (_events.find(event.get_fd()) != _events.end()) {
        return;
    }

    auto event_ptr = std::make_unique<Event>(event);

    epoll_event ev;
    ev.events   = event_ptr->to_epoll();
    ev.data.ptr = event_ptr.get();

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, event_ptr->get_fd(), &ev) == -1) {
        throw std::runtime_error("Failed to add event to epoll instance");
    }

    _events[event.get_fd()] = std::move(event_ptr);
}

Poller& Poller::instance()
{
    static Poller instance;
    return instance;
}
}  // namespace webserv::async
