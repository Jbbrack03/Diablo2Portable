#include "utils/data_table_parser.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace d2::utils {

DataTable DataTableParser::parseExcel(const std::string& data) {
    DataTable table;
    
    if (data.empty()) {
        return table;
    }
    
    std::istringstream stream(data);
    std::string line;
    
    // Parse header line to get column names
    if (std::getline(stream, line)) {
        std::istringstream headerStream(line);
        std::string column;
        
        while (std::getline(headerStream, column, '\t')) {
            // Remove quotes if present
            if (column.size() >= 2 && column.front() == '"' && column.back() == '"') {
                column = column.substr(1, column.size() - 2);
            }
            table.columns_.push_back(column);
        }
    }
    
    // Parse data rows
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        
        DataTable::Row row;
        std::istringstream lineStream(line);
        std::string value;
        size_t columnIndex = 0;
        
        while (std::getline(lineStream, value, '\t') && columnIndex < table.columns_.size()) {
            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            row[table.columns_[columnIndex]] = value;
            columnIndex++;
        }
        
        // Only add rows that have data
        if (!row.empty()) {
            table.rows_.push_back(row);
        }
    }
    
    return table;
}

DataTable DataTableParser::parseExcelFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return DataTable();
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    return parseExcel(content);
}

bool DataTable::hasColumn(const std::string& columnName) const {
    return std::find(columns_.begin(), columns_.end(), columnName) != columns_.end();
}

DataTable::Row DataTable::findRow(const std::string& columnName, const std::string& value) const {
    for (const auto& row : rows_) {
        auto it = row.find(columnName);
        if (it != row.end() && it->second == value) {
            return row;
        }
    }
    return Row(); // Return empty row if not found
}

int DataTable::getIntValue(const Row& row, const std::string& columnName) const {
    auto it = row.find(columnName);
    if (it != row.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

float DataTable::getFloatValue(const Row& row, const std::string& columnName) const {
    auto it = row.find(columnName);
    if (it != row.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return 0.0f;
        }
    }
    return 0.0f;
}

} // namespace d2::utils