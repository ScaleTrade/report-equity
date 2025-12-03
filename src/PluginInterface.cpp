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
    std::string group_mask;
    if (request.HasMember("group") && request["group"].IsString()) {
        group_mask = request["group"].GetString();
    }

    try {

    } catch (const std::exception& e) {
        std::cerr << "[EquityReportInterface]: " << e.what() << std::endl;
    }

    const Node report = div({
        h1({text("Equity Report") }),
    });

    utils::CreateUI(report, response, allocator);
}