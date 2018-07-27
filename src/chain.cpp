#include <atomic>
#include <mutex>
#include <streams++/chain.hpp>
#include <thread>
#include <vector>
#include <workq/workq++.hpp>

namespace streams {

namespace internal {
class ChainPrivate {
public:
  ChainPrivate() {}

  std::atomic<bool> running = {false};
  std::mutex mutex;
  std::unique_ptr<PackageProducer> producer;
  std::vector<std::unique_ptr<PackageTransformer>> transformers;
};

class Task : public workq::Task {

public:
  Task(Package &&package, std::function<void(Package &package)> fn)
      : m_pack(std::move(package)), m_fn(std::move(fn)) {}

  void run() { m_fn(m_pack); }

private:
  Package m_pack;
  std::function<void(Package &package)> m_fn;
};

} // namespace internal

Chain::Chain() : d(new internal::ChainPrivate()) {}

Chain::~Chain() {}

Chain &Chain::add(std::unique_ptr<PackageTransformer> &&transformer) {
  if (d->running)
    return *this;
  std::unique_lock<std::mutex> lock(d->mutex);
  d->transformers.emplace_back(std::move(transformer));
  return *this;
}

Chain &Chain::set_producer(std::unique_ptr<PackageProducer> &&prod) {
  if (d->running)
    return *this;
  std::unique_lock<std::mutex> lock(d->mutex);
  d->producer = std::move(prod);
  return *this;
}

PackageProducer *Chain::producer() const {
  std::unique_lock<std::mutex> lock(d->mutex);
  if (!d->producer)
    return nullptr;
  return d->producer.get();
}

static bool
run_chain(Package &package,
          const std::vector<std::unique_ptr<PackageTransformer>> &fns) {
  for (auto &t : fns) {
    if (!t->transform(package)) {
      return false;
    }
  }
  return true;
}

void Chain::run() {
  if (d->running) {
    return;
  }

  d->running = true;

  workq::DispatchQueue queue("chain-queue", std::thread::hardware_concurrency(),
                             true);

  while (true) {
    Package pack;
    bool ret = d->producer->read(pack);
    if (!ret)
      break;
    queue.async<internal::Task>(std::move(pack), [this](auto &pack) {
      run_chain(pack, d->transformers);
    });
  }

  queue.resume().wait();
}

} // namespace streams