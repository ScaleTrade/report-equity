#pragma once

#include <string>
#include <Structures.hpp>

struct Total {
    std::string currency;
    double equity;
    double credit;
    double profit;
    double balance;
    double margin;
    double margin_free;
};

struct AverageEquity {
    EquityRecord sum{};
    size_t count = 0;
};