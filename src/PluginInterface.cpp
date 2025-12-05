#include "PluginInterface.hpp"

using namespace ast;

extern "C" void AboutReport(rapidjson::Value& request,
                            rapidjson::Value& response,
                            rapidjson::Document::AllocatorType& allocator,
                            CServerInterface* server) {
    response.AddMember("version", 1, allocator);
    response.AddMember("name", Value().SetString("Equity report", allocator), allocator);
    response.AddMember(
        "description",
        Value().SetString("The financial state of accounts at the end of each day. "
                          "The accounts are grouped according to their 'Comment' field value, allowing you to generate reports for customizable groups.",
                          allocator), allocator);
    response.AddMember("type", REPORT_RANGE_GROUP_TYPE, allocator);
}

extern "C" void DestroyReport() {}

extern "C" void CreateReport(rapidjson::Value& request,
                             rapidjson::Value& response,
                             rapidjson::Document::AllocatorType& allocator,
                             CServerInterface* server) {
    std::unordered_map<std::string, Total> totals_map;

    std::string group_mask;
    int from;
    int to;
    if (request.HasMember("group") && request["group"].IsString()) {
        group_mask = request["group"].GetString();
    }
    if (request.HasMember("from") && request["from"].IsNumber()) {
        from = request["from"].GetInt();
    }
    if (request.HasMember("to") && request["to"].IsNumber()) {
        to = request["to"].GetInt();
    }

    std::vector<EquityRecord> equity_vector;

    try {
        server->GetAccountsEquitiesByGroup(from, to, group_mask, &equity_vector);
    } catch (const std::exception& e) {
        std::cerr << "[EquityReportInterface]: " << e.what() << std::endl;
    }

    std::cout << "SIZE: " << equity_vector.size() << std::endl;

    TableBuilder table_builder("EquityReportTable");

    table_builder.SetIdColumn("login");
    table_builder.SetOrderBy("login", "DESC");
    table_builder.EnableRefreshButton(false);
    table_builder.EnableBookmarksButton(false);
    table_builder.EnableExportButton(true);

    table_builder.AddColumn({"login", "LOGIN"});
    table_builder.AddColumn({"create_time", "CREATE_TIME"});
    table_builder.AddColumn({"group", "GROUP"});
    table_builder.AddColumn({"leverage", "LEVERAGE"});
    table_builder.AddColumn({"balance", "BALANCE"});
    table_builder.AddColumn({"prevbalance", "PREV_BALANCE"});
    table_builder.AddColumn({"credit", "CREDIT"});
    table_builder.AddColumn({"equity", "EQUITY"});
    table_builder.AddColumn({"profit", "PROFIT"});
    table_builder.AddColumn({"storage", "SWAP"});
    table_builder.AddColumn({"commission", "COMMISSION"});
    table_builder.AddColumn({"margin", "MARGIN"});
    table_builder.AddColumn({"margin_free", "MARGIN_FREE"});
    table_builder.AddColumn({"margin_level", "MARGIN_LEVEL"});
    table_builder.AddColumn({"currency", "CURRENCY"});

    for (const auto& equity_record : equity_vector) {
        table_builder.AddRow({
            {"login", std::to_string(equity_record.login)},
            {"create_time", utils::FormatTimestampToString(equity_record.create_time)},
            {"group", equity_record.group},
            {"leverage", std::to_string(equity_record.leverage)},
            {"balance", std::to_string(equity_record.balance)},
            {"prevbalance", std::to_string(equity_record.prevbalance)},
            {"credit", std::to_string(equity_record.credit)},
            {"equity", std::to_string(equity_record.equity)},
            {"storage", std::to_string(equity_record.storage)},
            {"commission", std::to_string(equity_record.commission)},
            {"margin", std::to_string(equity_record.margin)},
            {"margin_free", std::to_string(equity_record.margin_free)},
            {"margin_level", std::to_string(equity_record.margin_level)},
        });
    }

    const JSONObject table_props = table_builder.CreateTableProps();
    const Node table_node = Table({}, table_props);

    const Node report = div({
        h1({text("Equity Report") }),
        table_node
    });

    utils::CreateUI(report, response, allocator);
}