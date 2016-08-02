#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include "result_benchmark.hpp"
#include "result_all.hpp"
#include "timer.hpp"
#include "types.hpp"

#include <vector>
#include <array>

namespace gearshifft {

  template<typename T_Context>
  class Application {
  public:
    /// Number of benchmark runs after warmup
    static constexpr int NR_RUNS = 5;
    static const int NR_RECORDS  = static_cast<int>(RecordType::_NrRecords);
    using ResultAllT = ResultAll<NR_RUNS, NR_RECORDS>;
    using ResultT    = ResultBenchmark<NR_RUNS, NR_RECORDS>;
    /// Boost tests will fail when deviation(iFFT(FFT(data)),data) returns a greater value
    static constexpr double ERROR_BOUND = 0.00001;

    static Application& getInstance() {
      static Application app;
      return app;
    }
    static T_Context& getContext() {
      return getInstance().context_;
    }

    void createContext() {
      TimerCPU timer;
      timer.startTimer();
      context_.create();
      timeContextCreate_ = timer.stopTimer();
    }

    void destroyContext() {
      TimerCPU timer;
      timer.startTimer();
      context_.destroy();
      timeContextDestroy_ = timer.stopTimer();
    }

    void addRecord(ResultT r) {
      resultAll_.add(r);
    }

    void dumpResults() {
      resultAll_.write(T_Context::title(),
                       context_.getDeviceInfos(),
                       timeContextCreate_,
                       timeContextDestroy_);
    }

  private:
    T_Context context_;
    ResultAllT resultAll_;
    ResultT result_;
    double timeContextCreate_ = 0.0;
    double timeContextDestroy_ = 0.0;

    Application() = default;
    ~Application() = default;
  };

  template<typename T_Context>
  struct FixtureApplication {
    using ApplicationT = Application<T_Context>;

    FixtureApplication() {
      ApplicationT::getInstance().createContext();
    }
    ~FixtureApplication() {
      ApplicationT::getInstance().destroyContext();
      ApplicationT::getInstance().dumpResults();
    }
  };

}

#endif