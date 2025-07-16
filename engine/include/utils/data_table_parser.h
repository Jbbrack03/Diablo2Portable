#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace d2::utils {

// Forward declaration
class DataTable;

/**
 * Parser for Diablo II's tab-delimited data tables (Excel files)
 * 
 * Diablo II uses tab-delimited text files for game data like items,
 * monsters, skills, etc. This parser converts these files into
 * easily queryable data structures.
 */
class DataTableParser {
public:
    DataTableParser() = default;
    ~DataTableParser() = default;
    
    /**
     * Parse a tab-delimited data table
     * @param data The raw text data to parse
     * @return A DataTable object containing the parsed data
     */
    DataTable parseExcel(const std::string& data);
    
    /**
     * Parse a data table from a file
     * @param filename Path to the file to parse
     * @return A DataTable object containing the parsed data
     */
    DataTable parseExcelFile(const std::string& filename);
};

/**
 * Represents a parsed data table
 */
class DataTable {
public:
    using Row = std::unordered_map<std::string, std::string>;
    
    DataTable() = default;
    ~DataTable() = default;
    
    /**
     * Get the number of rows in the table
     */
    size_t getRowCount() const { return rows_.size(); }
    
    /**
     * Check if a column exists
     */
    bool hasColumn(const std::string& columnName) const;
    
    /**
     * Find a row by column value
     * @param columnName The column to search in
     * @param value The value to search for
     * @return The first matching row, or empty row if not found
     */
    Row findRow(const std::string& columnName, const std::string& value) const;
    
    /**
     * Get all rows
     */
    const std::vector<Row>& getAllRows() const { return rows_; }
    
    /**
     * Get a row by index
     */
    const Row& getRow(size_t index) const { return rows_[index]; }
    
    /**
     * Get an integer value from a row
     * @param row The row to get the value from
     * @param columnName The column name
     * @return The integer value, or 0 if not found or not a number
     */
    int getIntValue(const Row& row, const std::string& columnName) const;
    
    /**
     * Get a float value from a row
     * @param row The row to get the value from
     * @param columnName The column name
     * @return The float value, or 0.0f if not found or not a number
     */
    float getFloatValue(const Row& row, const std::string& columnName) const;
    
    // Friend class for parser to access internals
    friend class DataTableParser;
    
private:
    std::vector<std::string> columns_;
    std::vector<Row> rows_;
};

} // namespace d2::utils