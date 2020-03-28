#include "metrics.h"

#include <iostream>

namespace prom {

using namespace prometheus;

auto prometheus_registry = std::make_shared<Registry>();

Family<Counter> &income_counter_family =
    BuildCounter()
        .Name("income_money")
        .Help("how much money this player has earned")
        .Register(*prometheus_registry);

Family<Counter> &expenses_counter_family =
    BuildCounter()
        .Name("expenses_money")
        .Help("how much money this player has spent")
        .Register(*prometheus_registry);

void RegisterMetrics() {
  static Exposer exposer{"127.0.0.1:8080", "/metrics", 1};
  exposer.RegisterCollectable(prometheus_registry);
}

CompanyMetrics::CompanyMetrics(uint16 name_1) {
  this->name = name_1;

  this->income_counter = std::shared_ptr<prometheus::Counter>(
      &income_counter_family.Add({{"company", std::to_string(this->name)}}));

  this->expenses_counter = std::shared_ptr<prometheus::Counter>(
      &expenses_counter_family.Add({{"company", std::to_string(this->name)}}));

  std::cout << "creatign metrics" << this->name << std::endl;
}

CompanyMetrics::~CompanyMetrics() {
  std::cout << "deleting metrics" << this->name << std::endl;
}

}  // namespace prom