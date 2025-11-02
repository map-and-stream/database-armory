#pragma once
#include <optional>
#include <sstream>
#include <string>
#include <vector>

enum class QueryType { SELECT, INSERT, UPDATE, DELETE, RAW };

class QueryBuilder {
  public:
    QueryBuilder& table(const std::string& t) {
        _table = t;
        return *this;
    }

    QueryBuilder& select(const std::string& col) {
        _type = QueryType::SELECT;
        _selects.push_back(col);
        return *this;
    }

    QueryBuilder& insert(const std::vector<std::string>& cols, const std::vector<std::string>& vals) {
        _type = QueryType::INSERT;
        _insertCols = cols;
        _insertVals = vals;
        return *this;
    }

    QueryBuilder& update(const std::vector<std::pair<std::string, std::string>>& sets) {
        _type = QueryType::UPDATE;
        _updates = sets;
        return *this;
    }

    QueryBuilder& remove() {
        _type = QueryType::DELETE;
        return *this;
    }

    QueryBuilder& raw(const std::string& sql) {
        _type = QueryType::RAW;
        _rawQuery = sql;
        return *this;
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

        if (_type == QueryType::RAW) {
            return _rawQuery;
        }

        switch (_type) {
            case QueryType::SELECT:
                os << "SELECT ";
                if (_selects.empty()) {
                    os << "*";
                } else {
                    for (size_t i = 0; i < _selects.size(); ++i) {
                        if (i) os << ", ";
                        os << _selects[i];
                    }
                }
                os << " FROM " << _table;
                break;

            case QueryType::INSERT:
                os << "INSERT INTO " << _table << " (";
                for (size_t i = 0; i < _insertCols.size(); ++i) {
                    if (i) os << ", ";
                    os << _insertCols[i];
                }
                os << ") VALUES (";
                for (size_t i = 0; i < _insertVals.size(); ++i) {
                    if (i) os << ", ";
                    os << "'" << escape(_insertVals[i]) << "'";
                }
                os << ")";
                break;

            case QueryType::UPDATE:
                os << "UPDATE " << _table << " SET ";
                for (size_t i = 0; i < _updates.size(); ++i) {
                    if (i) os << ", ";
                    os << _updates[i].first << " = '" << escape(_updates[i].second) << "'";
                }
                break;

            case QueryType::DELETE:
                os << "DELETE FROM " << _table;
                break;

            default:
                break;
        }

        if (!_wheres.empty()) {
            os << " WHERE ";
            for (size_t i = 0; i < _wheres.size(); ++i) {
                if (i) os << " AND ";
                os << _wheres[i];
            }
        }

        if (_type == QueryType::SELECT) {
            if (_orderBy) os << " ORDER BY " << *_orderBy;
            if (_limit) os << " LIMIT " << *_limit;
            if (_offset) os << " OFFSET " << *_offset;
        }

        return os.str();
    }

  private:
    QueryType _type = QueryType::SELECT;
    std::string _table;
    std::string _rawQuery;
    std::vector<std::string> _selects;
    std::vector<std::string> _insertCols;
    std::vector<std::string> _insertVals;
    std::vector<std::pair<std::string, std::string>> _updates;
    std::vector<std::string> _wheres;
    std::optional<std::string> _orderBy;
    std::optional<int> _limit;
    std::optional<int> _offset;

    static std::string escape(const std::string& value) {
        std::string escaped = value;
        size_t pos = 0;
        while ((pos = escaped.find('\'', pos)) != std::string::npos) {
            escaped.insert(pos, "'");
            pos += 2;
        }
        return escaped;
    }
};
