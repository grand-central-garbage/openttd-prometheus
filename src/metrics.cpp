#include "metrics.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "cargotype.h"
#include "stdafx.h"
#include "strings_func.h"

namespace prom {

using namespace prometheus;

auto prometheus_registry = std::make_shared<Registry>();

std::string game_name;

Family<Counter> &income_counter_family =
    BuildCounter()
        .Name("openttd_income_money")
        .Help("how much money this player has earned")
        .Register(*prometheus_registry);

Family<Counter> &expenses_counter_family =
    BuildCounter()
        .Name("openttd_expenses_money")
        .Help("how much money this player has spent")
        .Register(*prometheus_registry);

Family<Counter> &cargo_delivered_family =
    BuildCounter()
        .Name("openttd_cargo_delivered")
        .Help("how much cargo this player has delivered")
        .Register(*prometheus_registry);

Family<Counter> &cargo_delivered_income_family =
    BuildCounter()
        .Name("openttd_cargo_delivered_income")
        .Help("how much income this player has earned from a cargo type")
        .Register(*prometheus_registry);

void RegisterMetrics() {
  static Exposer exposer{"127.0.0.1:10808", "/metrics", 1};
  exposer.RegisterCollectable(prometheus_registry);

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "openttd-%d-%m-%Y-%H-%M-%S");
  game_name = oss.str();
}

CompanyMetrics::CompanyMetrics(uint16 name_1) {
  this->name = name_1;
  auto company_name = std::to_string(this->name);

  this->income_counter =
      std::shared_ptr<prometheus::Counter>(&income_counter_family.Add(
          {{"game", game_name}, {"company", company_name}}));

  this->expenses_counter =
      std::shared_ptr<prometheus::Counter>(&expenses_counter_family.Add(
          {{"game", game_name}, {"company", company_name}}));

  const CargoSpec *cargo;
  FOR_ALL_CARGOSPECS(cargo) {
    auto cargo_type = std::string(GetStringPtr(cargo->name));
    this->cargo_delivered_counters[cargo->label] =
        std::shared_ptr<prometheus::Counter>(
            &cargo_delivered_family.Add({{"game", game_name},
                                         {"company", company_name},
                                         {"cargo_type", cargo_type}}));

    this->cargo_delivered_income_counters[cargo->label] =
        std::shared_ptr<prometheus::Counter>(
            &cargo_delivered_income_family.Add({{"game", game_name},
                                                {"company", company_name},
                                                {"cargo_type", cargo_type}}));
  }

  std::cout << "creatign metrics" << this->name << std::endl;
}

CompanyMetrics::~CompanyMetrics() {
  std::cout << "deleting metrics" << this->name << std::endl;
}

void CompanyMetrics::increment_cargo_delivered(CargoLabel label,
                                               double amount) {
  auto delivered = this->cargo_delivered_counters[label];
  delivered->Increment(amount);
}

void CompanyMetrics::increment_cargo_delivered_income(CargoLabel label,
                                                      double amount) {
  auto income = this->cargo_delivered_income_counters[label];
  income->Increment(amount);
}

}  // namespace prom