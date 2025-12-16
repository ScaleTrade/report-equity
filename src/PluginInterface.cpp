#include "PluginInterface.hpp"

using namespace ast;

extern "C" void AboutReport(rapidjson::Value& request,
                            rapidjson::Value& response,
                            rapidjson::Document::AllocatorType& allocator,
                            CServerInterface* server) {
    response.AddMember("version", 1, allocator);
    response.AddMember("name", Value().SetString("Daily Equity report", allocator), allocator);
    response.AddMember(
        "description",
        Value().SetString("The financial state of accounts at the end of each day. "
                          "The accounts are grouped according to their 'Comment' field value, allowing you to generate reports for customizable groups.",
                          allocator), allocator);
    response.AddMember("type", REPORT_DAILY_GROUP_TYPE, allocator);
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
    std::vector<AccountRecord> account_vector;

    try {
        server->GetAccountsEquitiesByGroup(from, to, group_mask, &equity_vector);
    } catch (const std::exception& e) {
        std::cerr << "[EquityReportInterface]: " << e.what() << std::endl;
    }

    std::cout << "Equity vector SIZE: " << equity_vector.size() << std::endl;

    // v.1
    // TableBuilder table_builder("EquityReportTable");
    // table_builder.SetIdColumn("login");
    // table_builder.SetOrderBy("login", "DESC");
    // table_builder.EnableRefreshButton(false);
    // table_builder.EnableBookmarksButton(false);
    // table_builder.EnableExportButton(true);
    // table_builder.EnableTotal(true);
    // table_builder.SetTotalDataTitle("TOTAL");
    //
    // table_builder.AddColumn({"login", "LOGIN", 1});
    // table_builder.AddColumn({"create_time", "CREATE_TIME", 2});
    // table_builder.AddColumn({"group", "GROUP", 3});
    // table_builder.AddColumn({"leverage", "LEVERAGE", 4});
    // table_builder.AddColumn({"balance", "BALANCE", 5});
    // table_builder.AddColumn({"prevbalance", "PREV_BALANCE", 6});
    // table_builder.AddColumn({"credit", "CREDIT", 7});
    // table_builder.AddColumn({"equity", "EQUITY", 8});
    // table_builder.AddColumn({"profit", "PROFIT", 9});
    // table_builder.AddColumn({"storage", "SWAP", 10});
    // table_builder.AddColumn({"commission", "COMMISSION", 11});
    // table_builder.AddColumn({"margin", "MARGIN", 12});
    // table_builder.AddColumn({"margin_free", "MARGIN_FREE", 13});
    // table_builder.AddColumn({"margin_level", "MARGIN_LEVEL", 14});
    // table_builder.AddColumn({"currency", "CURRENCY", 15});
    //
    // for (const auto& equity_record : equity_vector) {
    //     table_builder.AddRow({
    //         {"login", utils::TruncateDouble(equity_record.login, 0)},
    //         {"create_time", utils::FormatTimestampToString(equity_record.create_time)},
    //         {"group", equity_record.group},
    //         {"leverage", utils::TruncateDouble(equity_record.leverage, 0)},
    //         {"balance", utils::TruncateDouble(equity_record.balance, 2)},
    //         {"prevbalance", utils::TruncateDouble(equity_record.prevbalance, 2)},
    //         {"credit", utils::TruncateDouble(equity_record.credit, 2)},
    //         {"equity", utils::TruncateDouble(equity_record.equity, 2)},
    //         {"storage", utils::TruncateDouble(equity_record.storage, 2)},
    //         {"commission", utils::TruncateDouble(equity_record.commission, 2)},
    //         {"margin", utils::TruncateDouble(equity_record.margin, 2)},
    //         {"margin_free", utils::TruncateDouble(equity_record.margin_free, 2)},
    //         {"margin_level", utils::TruncateDouble(equity_record.margin_level, 2)},
    //     });
    // }
    //
    // const JSONObject table_props = table_builder.CreateTablePropsCompact();



    // v.2
    TableBuilder table_builder("EquityReportTable");

    // Настройки
    table_builder.SetIdColumn("login");
    table_builder.SetOrderBy("login", "DESC");
    table_builder.EnableRefreshButton(false);
    table_builder.EnableBookmarksButton(false);
    table_builder.EnableExportButton(true);
    table_builder.EnableTotal(true);
    table_builder.SetTotalDataTitle("TOTAL");

    // Колонки с order
    table_builder.AddColumn({"login", "LOGIN", 1});
    table_builder.AddColumn({"create_time", "CREATE_TIME", 2});
    table_builder.AddColumn({"balance", "BALANCE", 5});
    table_builder.AddColumn({"equity", "EQUITY", 8});

    // Добавление строк
    for (const auto& rec : equity_vector) {
        table_builder.AddRow({
            {"login", utils::TruncateDouble(rec.login, 0)},
            {"create_time", utils::FormatTimestampToString(rec.create_time)},
            {"balance", utils::TruncateDouble(rec.balance, 2)},
            {"equity", utils::TruncateDouble(rec.equity, 2)}
        });
    }

    const JSONObject table_props = table_builder.CreateTableProps();
    const Node table_node = Table({}, table_props);

    const Node report = Column({
        h1({text("Daily Equity Report") }),
        table_node
    });

    utils::CreateUI(report, response, allocator);

    std::cout << "SUCCESS" << std::endl;
}