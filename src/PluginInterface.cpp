#include "PluginInterface.h"

using namespace ast;

extern "C" void AboutReport(rapidjson::Value&                   request,
                            rapidjson::Value&                   response,
                            rapidjson::Document::AllocatorType& allocator,
                            CServerInterface*                   server) {
    response.AddMember("version", 1, allocator);
    response.AddMember("name", Value().SetString("Daily Equity report", allocator), allocator);
    response.AddMember(
        "description",
        Value().SetString("The financial state of accounts at the end of each day. "
                          "The accounts are grouped according to their 'Group' field value, "
                          "allowing you to generate reports for customizable groups.",
                          allocator),
        allocator);
    response.AddMember("type", REPORT_DAILY_GROUP_TYPE, allocator);
}

extern "C" void DestroyReport() {}

extern "C" void CreateReport(rapidjson::Value&                   request,
                             rapidjson::Value&                   response,
                             rapidjson::Document::AllocatorType& allocator,
                             CServerInterface*                   server) {
    std::string group_mask;
    int         from;
    int         to;
    if (request.HasMember("group") && request["group"].IsString()) {
        group_mask = request["group"].GetString();
    }
    if (request.HasMember("from") && request["from"].IsNumber()) {
        from = request["from"].GetInt();
    }
    if (request.HasMember("to") && request["to"].IsNumber()) {
        to = request["to"].GetInt();
    }

    std::vector<EquityRecord>              equity_vector;
    std::vector<GroupRecord>               group_vector;
    std::unordered_map<std::string, Total> totals_map;

    try {
        server->GetAccountsEquitiesByGroup(from, to, group_mask, &equity_vector);
        server->GetAllGroups(&group_vector);
    } catch (const std::exception& e) {
        std::cerr << "[DailyEquityReportInterface]: " << e.what() << std::endl;
    }

    // Main table
    TableBuilder table_builder("DailyEquityReportTable");

    // Main table props
    table_builder.SetIdColumn("login");
    table_builder.SetOrderBy("login", "DESC");
    table_builder.EnableAutoSave(false);
    table_builder.EnableRefreshButton(false);
    table_builder.EnableBookmarksButton(false);
    table_builder.EnableExportButton(true);
    table_builder.EnableTotal(true);
    table_builder.SetTotalDataTitle("TOTAL");

    // Filters
    FilterConfig search_filter;
    search_filter.type = FilterType::Search;

    FilterConfig date_time_filter;
    date_time_filter.type = FilterType::DateTime;

    FilterConfig group_select_filter;
    group_select_filter.type = FilterType::Select;
    for (const auto& group : group_vector) {
        group_select_filter.options.push_back({group.group, group.group});
    }

    // Columns
    table_builder.AddColumn({"login", "LOGIN", 1, search_filter});
    table_builder.AddColumn({"create_time", "CREATE_TIME", 2, date_time_filter});
    table_builder.AddColumn({"group", "GROUP", 3, group_select_filter});
    table_builder.AddColumn({"balance", "BALANCE", 5, search_filter});
    table_builder.AddColumn({"prevbalance", "PREV_BALANCE", 6, search_filter});
    table_builder.AddColumn({"floating_pl", "FLOATING_PL", 7, search_filter});
    table_builder.AddColumn({"credit", "CREDIT", 8, search_filter});
    table_builder.AddColumn({"equity", "EQUITY", 9, search_filter});
    table_builder.AddColumn({"profit", "AMOUNT", 10, search_filter});
    table_builder.AddColumn({"storage", "SWAP", 11, search_filter});
    table_builder.AddColumn({"commission", "COMMISSION", 12, search_filter});
    table_builder.AddColumn({"margin", "MARGIN", 13, search_filter});
    table_builder.AddColumn({"margin_free", "MARGIN_FREE", 14, search_filter});
    table_builder.AddColumn({"margin_level", "MARGIN_LEVEL (%)", 15, search_filter});
    table_builder.AddColumn({"currency", "CURRENCY", 16, search_filter});

    totals_map["USD"].currency = "USD";

    for (const auto& equity_record : equity_vector) {
        double floating_pl = 0.0;

        double multiplier = 1.0;

        if (equity_record.currency != "USD") {
            try {
                server->CalculateConvertRateByCurrency(
                    equity_record.currency, "USD", OP_SELL, &multiplier);
            } catch (const std::exception& e) {
                std::cerr << "[DailyEquityReportInterface]: " << e.what() << std::endl;
            }
        }

        floating_pl = equity_record.equity - equity_record.balance;

        totals_map["USD"].equity += equity_record.equity * multiplier;
        totals_map["USD"].credit += equity_record.credit * multiplier;
        totals_map["USD"].floating_pl += floating_pl * multiplier;
        totals_map["USD"].profit += equity_record.profit * multiplier;
        totals_map["USD"].balance += equity_record.balance * multiplier;
        totals_map["USD"].prevbalance += equity_record.prevbalance * multiplier;
        totals_map["USD"].storage += equity_record.storage * multiplier;
        totals_map["USD"].commission += equity_record.commission * multiplier;
        totals_map["USD"].margin += equity_record.margin * multiplier;
        totals_map["USD"].margin_free += equity_record.margin_free * multiplier;

        table_builder.AddRow({utils::TruncateDouble(equity_record.login, 0),
                              utils::FormatTimestampToString(equity_record.create_time),
                              equity_record.group,
                              utils::TruncateDouble(equity_record.balance * multiplier, 2),
                              utils::TruncateDouble(equity_record.prevbalance * multiplier, 2),
                              utils::TruncateDouble(floating_pl * multiplier, 2),
                              utils::TruncateDouble(equity_record.credit * multiplier, 2),
                              utils::TruncateDouble(equity_record.equity * multiplier, 2),
                              utils::TruncateDouble(equity_record.profit * multiplier, 2),
                              utils::TruncateDouble(equity_record.storage * multiplier, 2),
                              utils::TruncateDouble(equity_record.commission * multiplier, 2),
                              utils::TruncateDouble(equity_record.margin * multiplier, 2),
                              utils::TruncateDouble(equity_record.margin_free * multiplier, 2),
                              utils::TruncateDouble(equity_record.margin_level, 2),
                              "USD"});
    }

    // Total row
    JSONArray totals_array;
    totals_array.emplace_back(JSONObject{
        {"equity", utils::TruncateDouble(totals_map["USD"].equity, 2)},
        {"credit", utils::TruncateDouble(totals_map["USD"].credit, 2)},
        {"floating_pl", utils::TruncateDouble(totals_map["USD"].floating_pl, 2)},
        {"profit", utils::TruncateDouble(totals_map["USD"].profit, 2)},
        {"prevbalance", utils::TruncateDouble(totals_map["USD"].prevbalance, 2)},
        {"balance", utils::TruncateDouble(totals_map["USD"].balance, 2)},
        {"storage", utils::TruncateDouble(totals_map["USD"].storage, 2)},
        {"commission", utils::TruncateDouble(totals_map["USD"].commission, 2)},
        {"margin", utils::TruncateDouble(totals_map["USD"].margin, 2)},
        {"margin_free", utils::TruncateDouble(totals_map["USD"].margin_free, 2)},
        {"currency", totals_map["USD"].currency},
    });

    table_builder.SetTotalData(totals_array);

    const JSONObject table_props = table_builder.CreateTableProps();
    const Node       table_node  = Table({}, table_props);

    const Node report = Column({h1({text("Daily Equity Report")}), table_node});

    utils::CreateUI(report, response, allocator);
}