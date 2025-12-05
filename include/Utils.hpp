#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include "ast/Ast.hpp"
#include <rapidjson/document.h>
#include "Structures.hpp"

namespace utils {
    void CreateUI(const ast::Node& node,
              rapidjson::Value& response,
              rapidjson::Document::AllocatorType& allocator);

    std::string FormatTimestampToString(const time_t timestamp);

    std::vector<EquityRecord> AverageByLogin(const std::vector<EquityRecord>& records);

    // Костыль для показа
    std::vector<EquityRecord> GetFirst100Records(const std::vector<EquityRecord>& records);
}