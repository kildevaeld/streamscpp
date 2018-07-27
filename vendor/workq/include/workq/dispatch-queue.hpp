#pragma once
#include <memory>
#include <workq/task.hpp>

namespace workq {

namespace internal {
class DispatchQueuePrivate;
}

class DispatchQueue {

public:
  DispatchQueue(size_t capacity = 2, bool paused = false);
  DispatchQueue(const std::string &name, size_t capacity, bool paused = false);
  ~DispatchQueue();

  DispatchQueue(const DispatchQueue &) = delete;
  DispatchQueue &operator=(const DispatchQueue &) = delete;

  DispatchQueue(DispatchQueue &&);
  DispatchQueue &operator=(DispatchQueue &&);

  DispatchQueue &pause();
  DispatchQueue &resume();

  bool is_paused() const;

  template <typename T> void async(T fn) {
    Task *task = new GenericTask<T>(std::move(fn));
    async(task);
  }

  template <class T, typename... Args> void async(Args... args) {
    Task *task = new T(std::forward<Args>(args)...);
    async(task);
  }

  void async(Task *task);
  void wait();

private:
  std::unique_ptr<internal::DispatchQueuePrivate> d;
};

template <> class GenericTask<DispatchQueue> : public Task {
public:
  GenericTask(DispatchQueue &&t) : queue(std::move(t)) { queue.pause(); }
  ~GenericTask(){};

  void run() override {
    queue.resume();
    queue.wait();
  }

private:
  DispatchQueue queue;
};

} // namespace workq