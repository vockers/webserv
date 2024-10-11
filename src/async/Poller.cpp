#include "async/Poller.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
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
        /*Event* event = static_cast<Event*>(events[i].data.ptr);*/
        Event* event = _events[events[i].data.fd].get();
        if (events[i].events & EPOLLIN) {
            Poll poll = event->poll();
            if (poll == Poll::READY) {
                epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, event->get_fd(), nullptr);
                _events.erase(event->get_fd());
            }
        } else if (events[i].events & EPOLLOUT) {
            event->poll();
        }
    }

    for (auto it = _blocking_promises.begin(); it != _blocking_promises.end(); it++) {
        Poll poll = (*it)->poll();
        if (poll == Poll::READY) {
            _blocking_promises.erase(it);
            break;
        }
    }
}

void Poller::add_promise(std::unique_ptr<IPromise> promise, int fd, Event::Type type)
{
    auto event_ptr = std::make_unique<Event>(Event(fd, type, std::move(promise)));

    if (_events.find(fd) == _events.end()) {
        epoll_event ev;
        ev.events  = event_ptr->to_epoll();
        ev.data.fd = fd;

        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, event_ptr->get_fd(), &ev) == -1) {
            throw std::runtime_error("Failed to add event to epoll instance");
        }
    }

    _events[fd] = std::move(event_ptr);
}

void Poller::add_promise(std::unique_ptr<IPromise> promise)
{
    _blocking_promises.push_back(std::move(promise));
}

void Poller::add_event(Event event)
{
    if (_events.find(event.get_fd()) != _events.end()) {
        return;
    }
}

Poller& Poller::instance()
{
    static Poller instance;
    return instance;
}
}  // namespace webserv::async
