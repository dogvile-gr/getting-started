#include <memory>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <concord/glog_init.hpp>
#include <concord/Computation.hpp>
#include <concord/time_utils.hpp>


class WordCounter final : public bolt::Computation {
  public:
  using CtxPtr = bolt::Computation::CtxPtr;

  virtual void init(CtxPtr ctx) override {
    LOG(INFO) << "Init word count sink";
  }

  virtual void processRecord(CtxPtr ctx, bolt::FrameworkRecord &&r) override {
    map_[r.key()]++;
    if(++counter_ % 1024 == 0) {
      std::stringstream ss;
      std::for_each(map_.begin(), map_.end(), [&ss](const auto &p) {
        ss << '(' << p.first << " -> " << p.second << ')';
      });
      LOG(INFO) << "Key values stored: " << ss.str();
    }
  }

  virtual void
  processTimer(CtxPtr ctx, const std::string &key, int64_t time) override {
    LOG(FATAL) << "sink does not impl processTimer";
  }

  virtual bolt::Metadata metadata() override {
    return {"word-counter", {"words"}, {}};
  }


  private:
  std::unordered_map<std::string, uint64_t> map_{};
  uint64_t counter_{0};
};

int main(int argc, char *argv[]) {
  bolt::logging::glog_init(argv[0]);
  bolt::client::serveComputation(std::make_shared<WordCounter>(), argc, argv);
  return 0;
}
