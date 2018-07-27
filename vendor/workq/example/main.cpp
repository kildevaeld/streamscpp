#include <chrono>
#include <iostream>
#include <thread>
#include <workq/workq++.hpp>

class Task : public workq::Task {

public:
  Task(const std::string &name, size_t duration = 800)
      : m_name(name), m_dur(duration) {}

  void run() {
    std::cout << "Name " << m_name << " Waiting for " << m_dur << " ms"
              << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(m_dur));
  }

private:
  std::string m_name;
  size_t m_dur;
};

int main() {
  workq::DispatchQueue q("q1", 4, true);

  auto cb = [](int i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300 * i));
    std::cout << "Thread " << i << " " << std::this_thread::get_id()
              << std::endl;
  };

  for (int i = 0; i < 10; i++) {
    q.async(std::bind(cb, i));
  }

  q.resume();
  workq::DispatchQueue q2("q2", 1, true);

  q2.async([]() { std::cout << "q2 Hello" << std::endl; });
  q2.async([]() { std::cout << "q2 Hello 2" << std::endl; });

  q.async(std::move(q2));
  q2.async<Task>("Bjarne Hovsasa 2 ", 400);
  q.async<Task>("Bjarne Hovsasa");

  q.wait();
  std::cout << "Wait " << std::this_thread::get_id() << std::endl;

  return 0;
}