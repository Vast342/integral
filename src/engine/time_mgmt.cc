#include "time_mgmt.h"

#include "../chess/uci.h"
#include "../tuner/spsa.h"
#include "search.h"

Tunable base_time_scale("base_time_scale", 54, 0, 1000, 20, 0.0020);
Tunable increment_scale("increment_scale", 85, 0, 100, 5, 0.0020);
Tunable percent_limit("percent_limit", 76, 0, 1000, 20, 0.0020);
Tunable hard_limit_scale("hard_limit_scale", 304, 100, 450, 15, 0.0020);
Tunable soft_limit_scale("soft_limit_scale", 76, 0, 150, 5, 0.0020);
Tunable node_fraction_base("node_fraction_base", 152, 50, 250, 8, 0.0020);
Tunable node_fraction_scale("node_fraction_scale", 174, 50, 250, 5, 0.0020);

[[maybe_unused]] TimeManagement::TimeManagement(const TimeConfig &config) : nodes_spent_({}) {
  SetConfig(config);
}

void TimeManagement::Start() {
  const int base_time = config_.time_left * (base_time_scale / 1000.0) +
                        config_.increment * (increment_scale / 100.0) -
                        uci::GetOption("Move Overhead").GetValue<int>();
  const auto maximum_time = percent_limit / 100.0 * config_.time_left;

  hard_limit_.store(
      std::min((hard_limit_scale / 100.0) * base_time, maximum_time));
  soft_limit_.store(
      std::min((soft_limit_scale / 100.0) * base_time, maximum_time));

  start_time_.store(GetCurrentTime());
  nodes_spent_.fill(0);
}

void TimeManagement::Stop() {
  end_time_.store(GetCurrentTime());
}

void TimeManagement::SetConfig(const TimeConfig &config) {
  config_ = config;
  // Determine the structure of the time management from the config
  if (config.infinite) {
    type_ = TimeType::kInfinite;
  } else if (config.depth != 0) {
    type_ = TimeType::kDepth;
  } else {
    type_ = TimeType::kTimed;
  }
}

int TimeManagement::GetSearchDepth() const {
  switch (type_) {
    case TimeType::kInfinite:
      return std::numeric_limits<int>::max();
    case TimeType::kDepth:
      return config_.depth;
    case TimeType::kTimed:
      return kMaxSearchDepth;
    default:
      // Silence compiler warnings
      return 0;
  }
}

bool TimeManagement::TimesUp() {
  if (type_ != TimeType::kTimed) {
    return false;
  }

  return TimeElapsed() >= hard_limit_;
}

bool TimeManagement::ShouldStop(Move best_move, U32 nodes_searched) {
  if (type_ != TimeType::kTimed) {
    return false;
  }

  if (config_.move_time != 0) {
    return TimesUp();
  }

  const auto percent_searched =
      NodesSpent(best_move) / std::max<double>(1, nodes_searched);
  const double percent_scale_factor =
      (node_fraction_base / 100.0 - percent_searched) *
      (node_fraction_scale / 100.0);
  const U32 optimal_limit =
      std::min<U32>(soft_limit_ * percent_scale_factor, hard_limit_);

  return TimeElapsed() >= optimal_limit;
}

U32 &TimeManagement::NodesSpent(Move move) {
  return nodes_spent_[move.GetData() & 4095];
}

U64 TimeManagement::TimeElapsed() {
  return std::max<U64>(1, GetCurrentTime() - start_time_.load());
}