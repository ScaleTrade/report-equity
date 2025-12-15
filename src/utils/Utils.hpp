#pragma once

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Structures.hpp"
#include "structures/PluginStructures.hpp"
#include "ast/Ast.hpp"
#include <rapidjson/document.h>

namespace utils {
    void CreateUI(const ast::Node& node,
              rapidjson::Value& response,
              rapidjson::Document::AllocatorType& allocator);

    std::string FormatTimestampToString(const time_t& timestamp);

    double TruncateDouble(const double& value, const int& digits);

    std::string GetGroupCurrencyByName(const std::vector<GroupRecord>& group_vector, const std::string& group_name);

    std::vector<EquityRecord> AggregateAverageEquityByLogin(const std::vector<EquityRecord>& records);

    // Костыль для показа
    std::vector<EquityRecord> GetFirst100Records(const std::vector<EquityRecord>& records);
}