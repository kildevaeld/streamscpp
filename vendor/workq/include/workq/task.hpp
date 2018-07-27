#pragma once

namespace workq {

class Task {

public:
  virtual ~Task() {}
  virtual void run() = 0;
};

template <typename T> class GenericTask : public Task {

public:
  GenericTask(T &&t) : task(std::move(t)) {}
  ~GenericTask(){};

  void run() override { task(); }

private:
  T task;
};

template <> class GenericTask<std::function<void()>> : public Task {
public:
  GenericTask(std::function<void()> &&t) : task(std::move(t)) {}
  ~GenericTask(){};

  void run() override { task(); }

private:
  std::function<void()> task;
};

} // namespace workq