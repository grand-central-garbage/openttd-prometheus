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

namespace prom {
using namespace prometheus;

extern Family<Counter>& cash_counter_family;

void RegisterMetrics();

class CompanyMetrics {
 private:
  uint16 name;

 public:
  CompanyMetrics(uint16 name_1);
  ~CompanyMetrics();

  std::shared_ptr<prometheus::Counter> income_counter;
  std::shared_ptr<prometheus::Counter> expenses_counter;
};
}  // namespace prom

#endif /* METRICS_H */