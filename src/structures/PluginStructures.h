#pragma once

#include <string>

#include <Structures.h>

struct Total {
    std::string currency;
    double      equity;
    double      credit;
    double      floating_pl;
    double      profit;
    double      prevbalance;
    double      balance;
    double      storage;
    double      commission;
    double      margin;
    double      margin_free;
};