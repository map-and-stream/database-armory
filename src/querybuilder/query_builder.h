#pragma once
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

class QueryBuilder {
  public:
   
     // Table name
     
    // In QueryBuilder class

    // for writing insert method i add these two type of vector 
    std::vector<std::string> _insertColumns;
    std::vector<std::string> _insertValues;


     // Column-value pairs to update
     std::vector<std::pair<std::string, std::string>> _updates;

     //for Updating Row in sqlite i add these lines of code
     enum class QueryType { None, Select, Update, Delete, Insert };
    QueryType _type = QueryType::None;



    
     //WHERE clause
     std::string _where;
     QueryBuilder& set(const std::string& column, const std::string& value) {
        _updates.emplace_back(column, value);
        return *this;
    }
   

    //for removing row in users  table i have added these QueryBuilder Method

    QueryBuilder& remove(const std::string& t) {
        _table = t;
        _type = QueryType::Delete;
        return *this;
    }

    // i have added these two QueryBuilder for inserting

    QueryBuilder& insert(const std::string& t) {
        _table = t;
        _type = QueryType::Insert;
        return *this;
    }
    
    QueryBuilder& values(const std::vector<std::pair<std::string, std::string>>& colsVals) {
        for (auto& cv : colsVals) {
            _insertColumns.push_back(cv.first);
            _insertValues.push_back(cv.second);
        }
        return *this;
    }

    // Add WHERE condition
    
    QueryBuilder& update(const std::string& t) {
        // _table = t;
        // return *this;
        _table = t;
        _type = QueryType::Update;
        return *this;
        
    }
    QueryBuilder& table(const std::string& t) {
        _table = t;
        return *this;
        _table = t;
        _type = QueryType::Select; // default to SELECT if you call table()
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
    
        // for handling insert data to table i have added this iterator (conditional loops)
        if (_type == QueryType::Insert) {
            std::ostringstream os;
            os << "INSERT INTO " << _table << " (";
            for (size_t i = 0; i < _insertColumns.size(); ++i) {
                if (i) os << ", ";
                os << _insertColumns[i];
            }
            os << ") VALUES (";
            for (size_t i = 0; i < _insertValues.size(); ++i) {
                if (i) os << ", ";
                os << "'" << _insertValues[i] << "'";
            }
            os << ")";
            return os.str();
        }
        

        // for handling Renove row  i have added this iterator (conditional loops)

        if (_type == QueryType::Delete) {
            std::ostringstream os;
            os << "DELETE FROM " << _table;
            if (!_wheres.empty()) {
                os << " WHERE ";
                for (size_t i = 0; i < _wheres.size(); ++i) {
                    if (i) os << " AND ";
                    os << _wheres[i];
                }
            }
            return os.str();
        }
        

        // for handling Update Table and row to table i have added this iterator (conditional loops)

        if (_type == QueryType::Update) {
            os << "UPDATE " << _table << " SET ";
            for (size_t i = 0; i < _updates.size(); ++i) {
                os << _updates[i].first << "='" << _updates[i].second << "'";
                if (i + 1 < _updates.size()) os << ", ";
            }
            if (!_wheres.empty()) {
                os << " WHERE ";
                for (size_t i = 0; i < _wheres.size(); ++i) {
                    if (i) os << " AND ";
                    os << _wheres[i];
                }
            }
            return os.str();
        }
    
        // Otherwise, SELECT
        os << "SELECT ";
        if (_selects.empty())
            os << "*";
        else {
            for (size_t i = 0; i < _selects.size(); ++i) {
                if (i) os << ", ";
                os << _selects[i];
            }
        }
    
        os << " FROM " << _table;
    
        for (auto& j : _joins) os << " " << j;
    
        if (!_wheres.empty()) {
            os << " WHERE ";
            for (size_t i = 0; i < _wheres.size(); ++i) {
                if (i) os << " AND ";
                os << _wheres[i];
            }
        }
    
        if (_orderBy) os << " ORDER BY " << *_orderBy;
        if (_limit)   os << " LIMIT " << *_limit;
        if (_offset)  os << " OFFSET " << *_offset;
    
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
