/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2. OpenTTD is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with OpenTTD. If not, see
 * <http://www.gnu.org/licenses/>.
 */

/** @file metrics.h Header file for prometheus metrics */

#ifndef METRICS_H
#define METRICS_H

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

#include "stdafx.h"
#include "cargotype.h"
#include "economy_type.h"
#include "group.h"
#include "vehicle_type.h"

namespace prom {
using namespace prometheus;

extern Family<Counter>& cash_counter_family;

void RegisterMetrics();

class CompanyMetrics {
 private:
  std::string name;

  std::map<std::pair<CargoLabel, VehicleType>,
           std::shared_ptr<prometheus::Counter>>
      cargo_delivered_counters;

  std::map<std::pair<CargoLabel, VehicleType>,
           std::shared_ptr<prometheus::Counter>>
      cargo_delivered_income_counters;

  std::map<VehicleType, std::shared_ptr<prometheus::Gauge>>
      vehicles_owned_family_gauges;

  std::map<ExpensesType, std::shared_ptr<prometheus::Counter>>
      vehicle_running_costs_family_counters;

  std::shared_ptr<prometheus::Gauge> bank_balance;

 public:
  CompanyMetrics(char* name);
  ~CompanyMetrics();

  std::shared_ptr<prometheus::Counter> income_counter;
  std::shared_ptr<prometheus::Counter> expenses_counter;
  std::shared_ptr<prometheus::Counter> trees_planted_expenses_counter;

  void increment_cargo_delivered(CargoLabel label, VehicleType type,
                                 double amount);
  void increment_cargo_delivered_income(CargoLabel label, VehicleType type,
                                        double amount);

  void increment_vehicle_running_costs(ExpensesType type, double amount);

  void update_vehicle_counts(GroupStatistics* gs);

  void update_bank_balance(Money money, Money current_loan);

  std::string get_company_name();
};
}  // namespace prom

#endif /* METRICS_H */