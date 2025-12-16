#pragma once

#include <string>
#include <vector>
#include "ast/Ast.hpp"

using namespace ast;

struct TableColumn {
    std::string key;
    std::string language_token;
    double order = 0.0;
    bool is_exported = true;
    bool is_sorted = true;

    TableColumn(const std::string& key,
                const std::string& language_token,
                const double& order = 0.0,
                const bool& is_exported = true,
                const bool& is_sorted = true)
        : key(key),
          language_token(language_token),
          order(order),
          is_exported(is_exported),
          is_sorted(is_sorted){}
};

class TableBuilder {
public:
    explicit TableBuilder(const std::string& table_name)
        : _table_name(table_name) {}

    void AddColumn(const TableColumn& column) {
        _column_order.push_back(column.key);

        JSONObject column_obj;
        column_obj["name"] = column.language_token;
        column_obj["order"] = column.order;
        column_obj["export"] = column.is_exported;
        column_obj["sort"] = column.is_sorted;

        // TODO: расширить фильтр
        column_obj["filter"] = JSONObject{{"type", "search"}};

        _structure[column.key] = std::move(column_obj);
    }

    void AddRow(const std::vector<JSONValue>& row_values) {
        JSONArray json_row;
        json_row.reserve(row_values.size());
        for (const auto& val : row_values) {
            json_row.push_back(val);
        }
        _rows.push_back(std::move(json_row));
    }

    void SetIdColumn(const std::string& id_column) { _id_column = id_column; }

    void SetOrderBy(const std::string& column, const std::string& order = "DESC") {
        _order_by = {column, order};
    }

    void EnableRefreshButton(const bool& enabled) { _show_refresh_button = enabled; }

    void EnableBookmarksButton(const bool& enabled) { _show_bookmarks_button = enabled; }

    void EnableExportButton(const bool& enabled) { _show_export_button = enabled; }

    void EnableTotal(const bool& enabled) { _show_total = enabled; }

    void SetTotalDataTitle(const std::string& title) { _total_data_title = title; }

    void SetTotalData(const JSONArray& total_data) { _total_data = total_data; }

    [[nodiscard]] JSONObject CreateTableProps() const {
        JSONObject table_props;
        table_props["name"] = _table_name;
        table_props["idCol"] = _id_column;
        table_props["orderBy"] = JSONArray{_order_by.first, _order_by.second};
        table_props["showRefreshBtn"] = _show_refresh_button;
        table_props["showBookmarksBtn"] = _show_bookmarks_button;
        table_props["showExportBtn"] = _show_export_button;
        table_props["showTotal"] = _show_total;
        table_props["totalDataTitle"] = _total_data_title;

        if (!_total_data.empty()) {
            table_props["totalData"] = _total_data;
        }

        JSONObject data_obj;
        JSONArray json_rows;
        json_rows.reserve(_rows.size());

        for (const auto& row : _rows) {
            json_rows.push_back(row);
        }

        data_obj["rows"] = std::move(json_rows);


        JSONArray structure_keys;
        structure_keys.reserve(_column_order.size());

        for (const auto& key : _column_order) {
            structure_keys.push_back(key);
        }

        data_obj["structure"] = std::move(structure_keys);
        table_props["data"] = std::move(data_obj);
        table_props["structure"] = _structure;

        return table_props;
    }

private:
    std::string _table_name;

    std::vector<std::string> _column_order;

    std::vector<JSONArray> _rows;
    JSONObject _structure;

    std::string _id_column;
    std::pair<std::string, std::string> _order_by{"id", "DESC"};

    bool _show_refresh_button = true;
    bool _show_bookmarks_button = true;
    bool _show_export_button = true;
    bool _show_total = false;
    std::string _total_data_title;
    JSONArray _total_data;
};
