#include "Utils.hpp"

namespace utils {
    void CreateUI(const ast::Node& node,
                  rapidjson::Value& response,
                  rapidjson::Document::AllocatorType& allocator) {
        // Content
        Value node_object(kObjectType);
        to_json(node, node_object, allocator);

        Value content_array(kArrayType);
        content_array.PushBack(node_object, allocator);

        // Header
        Value header_array(kArrayType);

        {
            Value space_object(kObjectType);
            space_object.AddMember("type", "Space", allocator);

            Value children(kArrayType);

            Value text_object(kObjectType);
            text_object.AddMember("type", "#text", allocator);

            Value props(kObjectType);
            props.AddMember("value", "Pending Trades report", allocator);

            text_object.AddMember("props", props, allocator);
            children.PushBack(text_object, allocator);

            space_object.AddMember("children", children, allocator);

            header_array.PushBack(space_object, allocator);
        }

        // Footer
        Value footer_array(kArrayType);

        {
            Value space_object(kObjectType);
            space_object.AddMember("type", "Space", allocator);

            Value props_space(kObjectType);
            props_space.AddMember("justifyContent", "space-between", allocator);
            space_object.AddMember("props", props_space, allocator);

            Value children(kArrayType);

            Value btn_object(kObjectType);
            btn_object.AddMember("type", "Button", allocator);

            Value btn_props_object(kObjectType);
            btn_props_object.AddMember("className", "form_action_button", allocator);
            btn_props_object.AddMember("borderType", "danger", allocator);
            btn_props_object.AddMember("buttonType", "outlined", allocator);

            btn_props_object.AddMember("onClick", "{\"action\":\"CloseModal\"}", allocator);

            btn_object.AddMember("props", btn_props_object, allocator);

            Value btn_children(kArrayType);

            Value text_object(kObjectType);
            text_object.AddMember("type", "#text", allocator);

            Value text_props_object(kObjectType);
            text_props_object.AddMember("value", "Close", allocator);

            text_object.AddMember("props", text_props_object, allocator);
            btn_children.PushBack(text_object, allocator);

            btn_object.AddMember("children", btn_children, allocator);

            children.PushBack(btn_object, allocator);

            space_object.AddMember("children", children, allocator);

            footer_array.PushBack(space_object, allocator);
        }

        // Modal
        Value model_object(kObjectType);
        model_object.AddMember("size", "xxxl", allocator);
        model_object.AddMember("headerContent", header_array, allocator);
        model_object.AddMember("footerContent", footer_array, allocator);
        model_object.AddMember("content", content_array, allocator);


        // UI
        Value ui_object(kObjectType);
        ui_object.AddMember("modal", model_object, allocator);

        response.SetObject();
        response.AddMember("ui", ui_object, allocator);
    }

    std::string FormatTimestampToString(const time_t& timestamp) {
        std::tm tm{};
        localtime_r(&timestamp, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y.%m.%d %H:%M:%S");
        return oss.str();
    }

    double TruncateDouble(const double& value, const int& digits) {
        const double factor = std::pow(10.0, digits);
        return std::trunc(value * factor) / factor;
    }

    std::string GetGroupCurrencyByName(const std::vector<GroupRecord>& group_vector, const std::string& group_name) {
        for (const auto& group : group_vector) {
            if (group.group == group_name) {
                return group.currency;
            }
        }
        return "N/A";   // группа не найдена - валюта не определена
    }

    std::vector<EquityRecord> AggregateAverageEquityByLogin(const std::vector<EquityRecord>& records) {
        std::unordered_map<int, AverageEquity> temp;
        temp.reserve(records.size() / 4);

        for (const auto& equity_record : records) {
            auto [it, inserted] = temp.try_emplace(equity_record.login);
            auto& average_equity = it->second;

            average_equity.count++;
            average_equity.sum.balance += equity_record.balance;
            average_equity.sum.prevbalance += equity_record.prevbalance;
            average_equity.sum.credit += equity_record.credit;
            average_equity.sum.equity += equity_record.equity;
            average_equity.sum.profit += equity_record.profit;
            average_equity.sum.storage += equity_record.storage;
            average_equity.sum.commission += equity_record.commission;
            average_equity.sum.margin += equity_record.margin;
            average_equity.sum.margin_free += equity_record.margin_free;
            average_equity.sum.margin_level += equity_record.margin_level;

            if (average_equity.count == 1) {
                average_equity.sum.login = equity_record.login;
                average_equity.sum.create_time = equity_record.create_time;
                average_equity.sum.group = equity_record.group;
                average_equity.sum.leverage = equity_record.leverage;
                average_equity.sum.currency = equity_record.currency;
            }
        }

        std::vector<EquityRecord> result;
        result.reserve(temp.size());

        for (auto& [login, average_equity] : temp) {
            const double k = 1.0 / average_equity.count;
            EquityRecord equity_record = average_equity.sum;

            equity_record.balance *= k;
            equity_record.prevbalance *= k;
            equity_record.credit *= k;
            equity_record.equity *= k;
            equity_record.profit *= k;
            equity_record.storage *= k;
            equity_record.commission *= k;
            equity_record.margin *= k;
            equity_record.margin_free *= k;
            equity_record.margin_level *= k;

            result.emplace_back(std::move(equity_record));
        }

        return result;
    }

    // Костыль для показа
    std::vector<EquityRecord> GetFirst100Records(const std::vector<EquityRecord>& records) {
        const size_t limit = 100;

        if (records.size() <= limit) {
            return records;
        }

        return std::vector<EquityRecord>(records.begin(), records.begin() + limit);
    }
}
