#include "metrics.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "cargotype.h"
#include "company_base.h"
#include "stdafx.h"
#include "strings_func.h"
#include "vehicle_type.h"

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

Family<Gauge> &vehicles_owned_family =
    BuildGauge()
        .Name("openttd_vehicles_owned")
        .Help("how many vehicles this player owns")
        .Register(*prometheus_registry);

Family<Counter> &trees_planted_expenses_counter_family =
    BuildCounter()
        .Name("openttd_trees_expenses_money")
        .Help("how much money this player has spent on planting trees")
        .Register(*prometheus_registry);

Family<Gauge> &bank_balance_family =
    BuildGauge()
        .Name("openttd_bank_balance")
        .Help("the bank balance of this player (current money - current loan)")
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

CompanyMetrics::CompanyMetrics(char *name) {
  this->name = std::string(name);

  this->income_counter =
      std::shared_ptr<prometheus::Counter>(&income_counter_family.Add(
          {{"game", game_name}, {"company", this->name}}));

  this->expenses_counter =
      std::shared_ptr<prometheus::Counter>(&expenses_counter_family.Add(
          {{"game", game_name}, {"company", this->name}}));

  this->trees_planted_expenses_counter = std::shared_ptr<prometheus::Counter>(
      &trees_planted_expenses_counter_family.Add(
          {{"game", game_name}, {"company", this->name}}));

  this->bank_balance = std::shared_ptr<prometheus::Gauge>(
      &bank_balance_family.Add({{"game", game_name}, {"company", this->name}}));

  std::initializer_list<VehicleType> vehicle_types = {VEH_TRAIN, VEH_ROAD,
                                                      VEH_SHIP, VEH_AIRCRAFT};
  for (VehicleType vehicle_type : vehicle_types) {
    std::string transport_type;

    switch (vehicle_type) {
      case VEH_TRAIN:
        transport_type = "train";
        break;

      case VEH_ROAD:
        transport_type = "road";
        break;

      case VEH_AIRCRAFT:
        transport_type = "aircraft";
        break;

      case VEH_SHIP:
        transport_type = "ship";
        break;

      default:
        transport_type = "mystery";
        break;
    }

    this->vehicles_owned_family_gauges[vehicle_type] =
        std::shared_ptr<prometheus::Gauge>(
            &vehicles_owned_family.Add({{"game", game_name},
                                        {"company", this->name},
                                        {"transport_type", transport_type}}));

    const CargoSpec *cargo;
    FOR_ALL_CARGOSPECS(cargo) {
      auto cargo_type = std::string(GetStringPtr(cargo->name));

      this->cargo_delivered_counters[std::make_pair(cargo->label,
                                                    vehicle_type)] =
          std::shared_ptr<prometheus::Counter>(&cargo_delivered_family.Add(
              {{"game", game_name},
               {"company", this->name},
               {"cargo_type", cargo_type},
               {"transport_type", transport_type}}));

      this->cargo_delivered_income_counters[std::make_pair(cargo->label,
                                                           vehicle_type)] =
          std::shared_ptr<prometheus::Counter>(
              &cargo_delivered_income_family.Add(
                  {{"game", game_name},
                   {"company", this->name},
                   {"cargo_type", cargo_type},
                   {"transport_type", transport_type}}));
    }
  }

  std::cout << "creatign metrics" << this->name << std::endl;
}

CompanyMetrics::~CompanyMetrics() {
  std::cout << "deleting metrics" << this->name << std::endl;
}

void CompanyMetrics::increment_cargo_delivered(CargoLabel label,
                                               VehicleType type,
                                               double amount) {
  this->cargo_delivered_counters[std::make_pair(label, type)]->Increment(
      amount);
}

void CompanyMetrics::increment_cargo_delivered_income(CargoLabel label,
                                                      VehicleType type,
                                                      double amount) {
  this->cargo_delivered_income_counters[std::make_pair(label, type)]->Increment(
      amount);
}

void CompanyMetrics::update_vehicle_counts(GroupStatistics *gs) {
  std::initializer_list<VehicleType> vehicle_types = {VEH_TRAIN, VEH_ROAD,
                                                      VEH_SHIP, VEH_AIRCRAFT};
  for (VehicleType vehicle_type : vehicle_types) {
    this->vehicles_owned_family_gauges[vehicle_type]->Set(
        gs[vehicle_type].num_vehicle);
  }
}

void CompanyMetrics::update_bank_balance(Money money, Money current_loan) {
  this->bank_balance->Set(money - current_loan);
}

std::string CompanyMetrics::get_company_name() { return this->name; }

}  // namespace prom