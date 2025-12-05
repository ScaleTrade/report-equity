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
            props.AddMember("value", "Equity report", allocator);

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

    std::string FormatTimestampToString(const time_t timestamp) {
        std::tm tm{};
        localtime_r(&timestamp, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y.%m.%d %H:%M:%S");
        return oss.str();
    }

    std::vector<EquityRecord> AverageByLogin(const std::vector<EquityRecord>& records) {
        struct Agg {
            EquityRecord sum{};
            size_t count = 0;
        };

        std::unordered_map<int, Agg> temp;
        temp.reserve(records.size() / 4);

        for (const auto& r : records) {
            auto& a = temp[r.login];
            a.count++;

            a.sum.balance      += r.balance;
            a.sum.prevbalance  += r.prevbalance;
            a.sum.credit       += r.credit;
            a.sum.equity       += r.equity;
            a.sum.profit       += r.profit;
            a.sum.storage      += r.storage;
            a.sum.commission   += r.commission;
            a.sum.margin       += r.margin;
            a.sum.margin_free  += r.margin_free;
            a.sum.margin_level += r.margin_level;

            if (a.count == 1) {
                a.sum.login       = r.login;
                a.sum.create_time = r.create_time;
                a.sum.group       = r.group;
                a.sum.leverage    = r.leverage;
                a.sum.currency    = r.currency;
            }
        }

        std::vector<EquityRecord> result;
        result.reserve(temp.size());

        for (auto& [login, agg] : temp) {
            double k = 1.0 / agg.count;
            EquityRecord rec = agg.sum;

            rec.balance      *= k;
            rec.prevbalance  *= k;
            rec.credit       *= k;
            rec.equity       *= k;
            rec.profit       *= k;
            rec.storage      *= k;
            rec.commission   *= k;
            rec.margin       *= k;
            rec.margin_free  *= k;
            rec.margin_level *= k;

            result.push_back(std::move(rec));
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
