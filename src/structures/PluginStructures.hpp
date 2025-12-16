#pragma once

#include <string>
#include <Structures.hpp>

struct Total {
    std::string currency;
    double equity;
    double credit;
    double floating_pl;
    double profit;
    double prevbalance;
    double balance;
    double storage;
    double margin;
    double margin_free;
};

struct AverageEquity {
    EquityRecord sum{};
    size_t count = 0;
};