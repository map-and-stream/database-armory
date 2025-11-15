#pragma once

#include <string>
#include <vector>

class QueryResult {
  public:
    using Row = std::vector<std::string>;
    using Table = std::vector<Row>;

    QueryResult() = default;
    QueryResult(Table table, std::vector<std::string> columns)
        : table_(std::move(table)), columns_(std::move(columns)) {}

    bool empty() const { return table_.empty(); }
    size_t rows() const { return table_.size(); }
    size_t cols() const { return columns_.size(); }
    const std::vector<std::string>& columns() const { return columns_; }
    const Table& data() const { return table_; }

    // Optional: helper to get cell by (row, col)
    std::optional<std::string> at(size_t row, size_t col) const {
        if (row < table_.size() && col < table_[row].size()) {
            return table_[row][col];
        }
        return std::nullopt;
    }

    // ✅ New print() function
    void print(std::ostream& os = std::cout) const {
        if (columns_.empty()) {
            os << "(no columns)\n";
            return;
        }

        // Compute column widths
        std::vector<size_t> widths(columns_.size());
        for (size_t i = 0; i < columns_.size(); ++i) widths[i] = columns_[i].size();

        for (const auto& row : table_) {
            for (size_t i = 0; i < row.size(); ++i) widths[i] = std::max(widths[i], row[i].size());
        }

        // Print header
        os << "┌";
        for (size_t i = 0; i < widths.size(); ++i) {
            os << std::string(widths[i] + 2, '-');
            if (i < widths.size() - 1)
                os << "┬";
        }
        os << "┐\n";

        os << "│";
        for (size_t i = 0; i < columns_.size(); ++i)
            os << " " << std::setw(widths[i]) << std::left << columns_[i] << " │";
        os << "\n";

        os << "├";
        for (size_t i = 0; i < widths.size(); ++i) {
            os << std::string(widths[i] + 2, '-');
            if (i < widths.size() - 1)
                os << "┼";
        }
        os << "┤\n";

        // Print rows
        for (const auto& row : table_) {
            os << "│";
            for (size_t i = 0; i < columns_.size(); ++i) {
                const std::string& val = (i < row.size() ? row[i] : "");
                os << " " << std::setw(widths[i]) << std::left << val << " │";
            }
            os << "\n";
        }

        os << "└";
        for (size_t i = 0; i < widths.size(); ++i) {
            os << std::string(widths[i] + 2, '-');
            if (i < widths.size() - 1)
                os << "┴";
        }
        os << "┘\n";

        os << rows() << " rows returned.\n";
    }

  private:
    Table table_;
    std::vector<std::string> columns_;
};
