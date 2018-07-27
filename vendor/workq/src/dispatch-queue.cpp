#include "debug.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <workq/dispatch-queue.hpp>
#include <workq/task.hpp>

#include <iostream>

namespace workq {

namespace internal {

class DispatchQueuePrivate {

public:
  DispatchQueuePrivate(const std::string &n, size_t cap)
      : name(n), capacity(cap), threads(cap) {

    DEBUG("Creating dispatch queue: %s", name.c_str());
    DEBUG("%s#Dispatch threads: %zu", name.c_str(), cap);

    for (size_t i = 0; i < capacity; i++) {
      threads[i] =
          std::thread(std::bind(&DispatchQueuePrivate::dispatch_handler, this));
    }
  }

  ~DispatchQueuePrivate() {
    DEBUG("%s#Destructor: Destroying dispatch threads...", name.c_str());

    // Signal to dispatch threads that it's time to wrap up
    quit = true;
    condition.notify_all();

    // Wait for threads to finish before we exit
    for (size_t i = 0; i < threads.size(); i++) {
      if (threads[i].joinable()) {
        DEBUG("%s#Destructor: Joining thread %zu until completion",
              name.c_str(), i);
        threads[i].join();
      }
    }
    while (queue.size() > 0) {
      auto task = queue.front();
      queue.pop();
      delete task;
    }
  }
  size_t capacity;
  std::string name;
  std::mutex mutex;
  std::vector<std::thread> threads;
  std::condition_variable condition;
  std::queue<Task *> queue;
  std::atomic<bool> quit;
  std::atomic<bool> pause;
  std::atomic<size_t> running;

private:
  void dispatch_handler();
};

void DispatchQueuePrivate::dispatch_handler() {
  std::unique_lock<std::mutex> lock(mutex);

  do {
    // Wait until we have data or a quit signal
    condition.wait(lock, [this] {
      if (quit)
        return true;
      return (queue.size() > 0 && !pause);
    });

    // after wait, we own the lock
    if (queue.size() && !quit) {
      auto op = std::move(queue.front());
      queue.pop();

      // unlock now that we're done messing with the queue
      lock.unlock();

      running++;

      try {
        op->run();
      } catch (...) {
      }

      delete op;

      running--;
      condition.notify_all();

      lock.lock();

    } else if (!quit) {
      lock.unlock();
      condition.notify_all();
      lock.lock();
    }
  } while (!quit);
}

} // namespace internal

DispatchQueue &DispatchQueue::pause() {
  // std::unique_lock<std::mutex> lock(d->mutex);
  auto trigger = d->pause == false;
  if (trigger) {
    d->pause = true;
    // lock.unlock();
    d->condition.notify_all();
  }
  return *this;
}

DispatchQueue &DispatchQueue::resume() {
  // std::unique_lock<std::mutex> lock(d->mutex);
  auto trigger = d->pause == true;
  if (trigger) {
    d->pause = false;
    // lock.unlock();
    d->condition.notify_one();
  }
  return *this;
}

bool DispatchQueue::is_paused() const {
  // std::unique_lock<std::mutex> lock(d->mutex);
  return d->pause;
}

DispatchQueue::DispatchQueue(size_t cap, bool paused)
    : DispatchQueue("unknown", cap) {
  if (paused)
    pause();
}

DispatchQueue::DispatchQueue(const std::string &name, size_t capacity,
                             bool paused)
    : d(new internal::DispatchQueuePrivate(name, capacity)) {
  if (paused)
    pause();
}

DispatchQueue::DispatchQueue(DispatchQueue &&other) : d(std::move(other.d)) {}

DispatchQueue &DispatchQueue::operator=(DispatchQueue &&other) {
  if (this != &other) {
    d.swap(other.d);
  }

  return *this;
}

DispatchQueue::~DispatchQueue() {}

void DispatchQueue::async(Task *task) {
  std::unique_lock<std::mutex> lock(d->mutex);
  d->queue.push(task);
  lock.unlock();
  d->condition.notify_all();
}

void DispatchQueue::wait() {
  std::unique_lock<std::mutex> lock(d->mutex);
  d->condition.wait(
      lock, [this] { return (d->queue.size() == 0 && d->running == 0); });
}

} // namespace workq