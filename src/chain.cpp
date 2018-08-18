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
  ChainPrivate() { queue.pause(); }

  std::atomic<bool> running = {false};
  std::mutex mutex;
  std::vector<std::unique_ptr<PackageProducer>> producers;
  std::vector<std::unique_ptr<PackageTransformer>> transformers;
  workq::WorkQueue queue;
};

class Task : public workq::GenericTask {

public:
  Task(Package &&package, std::function<void(Package &package)> fn)
      : m_pack(std::move(package)), m_fn(std::move(fn)) {}

  void run() override { m_fn(m_pack); }
  // std::string name() const override { return "task"; }

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

Chain &Chain::add_producer(std::unique_ptr<PackageProducer> &&prod) {
  if (d->running)
    return *this;
  std::unique_lock<std::mutex> lock(d->mutex);
  d->producers.emplace_back(std::move(prod));
  return *this;
}

// PackageProducer *Chain::producer() const {
//   std::unique_lock<std::mutex> lock(d->mutex);
//   if (!d->producer)
//     return nullptr;
//   return d->producer.get();
// }

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

  if (d->producers.size() > 1) {
    for (int i = 0; i < d->producers.size(); i++) {
      d->queue.dispatch([i, this]() {
        auto &producer = d->producers[i];
        while (true) {
          Package pack;
          bool ret = producer->read(pack);
          if (!ret)
            break;
          d->queue.dispatch<internal::Task>(
              std::move(pack),
              [this](auto &pack) { run_chain(pack, d->transformers); });
        }
      });
    }
  } else if (d->producers.size() == 1) {
    auto &producer = d->producers[0];
    while (true) {
      Package pack;
      bool ret = producer->read(pack);
      if (!ret)
        break;
      d->queue.dispatch<internal::Task>(std::move(pack), [this](auto &pack) {
        run_chain(pack, d->transformers);
      });
    }
  }

  d->queue.resume().wait();
}

} // namespace streams