#pragma once
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class QueryBuilder {
  public:
    QueryBuilder& table(const std::string& t) {
        _table = t;
        return *this;
    }

    QueryBuilder& select(const std::string& col) {
        _selects.push_back(col);
        return *this;
    }

    QueryBuilder& join(const std::string& joinTable, const std::string& onLeft,
                       const std::string& onRight, const std::string& type = "INNER") {
        std::ostringstream os;
        os << type << " JOIN " << joinTable << " ON " << onLeft << " = " << onRight;
        _joins.push_back(os.str());
        return *this;
    }

    QueryBuilder& leftJoin(const std::string& joinTable, const std::string& onLeft,
                           const std::string& onRight) {
        return join(joinTable, onLeft, onRight, "LEFT");
    }

    QueryBuilder& rightJoin(const std::string& joinTable, const std::string& onLeft,
                            const std::string& onRight) {
        return join(joinTable, onLeft, onRight, "RIGHT");
    }

    QueryBuilder& where(const std::string& cond) {
        _wheres.push_back(cond);
        return *this;
    }

    QueryBuilder& orderBy(const std::string& expr) {
        _orderBy = expr;
        return *this;
    }

    QueryBuilder& limit(int n) {
        _limit = n;
        return *this;
    }

    QueryBuilder& offset(int n) {
        _offset = n;
        return *this;
    }

    std::string str() const {
        std::ostringstream os;
        os << "SELECT ";
        if (_selects.empty())
            os << "*";
        else {
            for (size_t i = 0; i < _selects.size(); ++i) {
                if (i)
                    os << ", ";
                os << _selects[i];
            }
        }

        os << " FROM " << _table;

        for (auto& j : _joins) os << " " << j;

        if (!_wheres.empty()) {
            os << " WHERE ";
            for (size_t i = 0; i < _wheres.size(); ++i) {
                if (i)
                    os << " AND ";
                os << _wheres[i];
            }
        }

        if (_orderBy)
            os << " ORDER BY " << *_orderBy;

        if (_limit)
            os << " LIMIT " << *_limit;

        if (_offset)
            os << " OFFSET " << *_offset;

        return os.str();
    }

  private:
    std::string _table;
    std::vector<std::string> _selects;
    std::vector<std::string> _joins;
    std::vector<std::string> _wheres;
    std::optional<std::string> _orderBy;
    std::optional<int> _limit;
    std::optional<int> _offset;
};
