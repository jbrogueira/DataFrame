#include "dataframe.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include "dataframeproxy.h"
#include "ConstColumnIterator.h"
class ColumnIterator;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

void DataFrame::missing_col_error(const char* what, const string& s) {
    std::string msg = " Column " + s + " not found";
    throw std::out_of_range(what + msg);
}
vector<int> DataFrame::get_index_positions(const vector<string>& inp) const {
    vector<int> res;
    std::transform(inp.begin(), inp.end(), std::back_inserter(res),
                   [this](const string& s) { return find_index_position(s); });
    return res;
}

vector<int> DataFrame::get_index_positions() const {
    vector<int> res;
    std::transform(index_names.begin(), index_names.end(), back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.second; });
    return res;
}

template <typename T>
vector<string> get_names(T& cont) {
    vector<string> res;
    std::transform(cont.begin(), cont.end(), std::back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.first; });
    return res;
}

vector<string> DataFrame::get_index_names() { return get_names(index_names); }
vector<string> DataFrame::get_index_names() const {
    return get_names(index_names);
}

vector<string> DataFrame::get_column_names() { return get_names(column_names); }
vector<string> DataFrame::get_column_names() const {
    return get_names(column_names);
}

DataFrame::DataFrame() : columns(), index_names(), column_names(){};

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_unique(s);  // Item 3
}

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) const {
    try {
        int idx = column_names.at(s);
        return make_shared<Column>(*columns.at(idx));
    } catch (const std::out_of_range& e) {
        string msg = "No column with name " + s;
        throw std::out_of_range(msg);
    }
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_shared_copy(s);  // Item 3
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) const {
    try {
        int idx = column_names.at(s);
        return columns[idx];
    } catch (const std::out_of_range& e) {
        string msg = "No column with name " + s;
        throw std::out_of_range(msg);
    }
}

shared_ptr<Column> DataFrame::get_unique(const string& s,
                                         const vector<int>& v) const {
    int i = column_names.at(s);
    Column new_col = Column(*columns[i], v);
    return make_shared<Column>(new_col);
}

const int DataFrame::find_index_position(const string& s) const {
    for (const auto& x : index_names) {
        if (x.first == s) return x.second;
    }
    return -1;
}

template <typename T>
int get_pair(const T& cont, const pair<string, int>& ele) {
    int pos = -1;
    for (const auto& x : cont) {
        if (x.first == ele.first && x.second == ele.second) return pos;
        pos++;
    }
    return pos;
}

int DataFrame::find_index_pair(const pair<string, int>& ele) {
    int pos = 0;
    for (const auto& x : index_names) {
        if (x.first == ele.first && x.second == ele.second) return pos;
        pos++;
    }
    throw std::out_of_range("Could not find the index argument");
}

const int DataFrame::find_index_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).find_index_position(s);
}

DataFrame deep_copy(const DataFrame& lhs) {
    DataFrame new_df = DataFrame();
    vector<int> old_positions = lhs.get_index_positions();
    int i = 0, j = 0;
    for (auto const& x : lhs.column_names) {
        new_df.column_names[x.first] = i++;
        new_df.columns.push_back(lhs.get_unique(x.first, old_positions));
    }
    for (auto const& x : lhs.index_names)
        new_df.index_names.push_back(make_pair(x.first, j++));
    return new_df;
}

template <typename T>
DataFrame::DataFrame(const vector<string>& idx, const vector<string>& n,
                     const vector<vector<T>>& cols) {
    if (n.size() != cols.size())
        throw std::invalid_argument("Column name len does not equal col len");
    for (size_t i = 0; i < n.size(); ++i) {
        if (idx.size() != cols[i].size())
            throw std::invalid_argument("Data and index has different length");
        columns.push_back(make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
    for (size_t i = 0; i < idx.size(); ++i)
        index_names.push_back(make_pair(idx[i], i));
}
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<double>>&);
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<string>>&);

DataFrame::DataFrame(const DataFrame::DataFrameProxy& df) {
    int i = 0;
    for (const string& name : df.colNames) {
        int pos = df.theDataFrame.column_names[name];
        columns.push_back(df.theDataFrame.columns[pos]);
        column_names[name] = i++;
    }
    for (const string& name : df.idxNames) {
        int pos = df.theDataFrame.find_index_position(name);
        index_names.push_back(make_pair(name, pos));
    }
}

const std::pair<int, int> DataFrame::size() const {
    return make_pair(index_names.size(), columns.size());
}

const int DataFrame::use_count(string col) {
    return columns[column_names[col]].use_count();
}

vector<pair<int, int>> correspondence_position(const DataFrame& lhs,
                                               const DataFrame& other) {
    vector<pair<int, int>> res;
    int i = 0;
    for (auto const& x : lhs.index_names) {
        int other_pos = other.find_index_position(x.first);
        res.push_back(make_pair(i++, other_pos));
    }
    return res;
}

void DataFrame::append_nan_rows() {
    for (auto& x : column_names) columns[x.second]->push_back_nan();
}

void DataFrame::append_index(const string& s) {
    index_names.push_back(make_pair(s, index_names.size()));
}

int DataFrame::get_column_position(const std::string& s) {
    try {
        return column_names.at(s);
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column " + s + " not found");
    }
}

bool DataFrame::contains_null(const string& s) {
    int pos = find_index_position(s);
    for (auto const& cols : column_names) {
        if (columns[cols.second]->is_null(pos))
            return true;
    }
    return false;
}

void DataFrame::make_unique(const std::string& s) {
    columns.at(column_names[s]) = get_unique(s);
}

void DataFrame::make_unique(const std::vector<string>& vec) {
    for (const string& s : vec) make_unique(s);
}

void DataFrame::make_unique_if(const std::string& s) {
    if (this->use_count(s) > 1) {
        std::cout << "copy-on-write\n";
        make_unique(s);
    }
}

void DataFrame::make_unique_if(const vector<string>& c) {
    auto fun = [&](int a, const string& s) { return a + use_count(s); };
    if (std::accumulate(c.begin(), c.end(), 0, fun) > size().second)
        make_unique(c);
}

void DataFrame::drop_row(const string& s) {
    int pos = find_index_pair(make_pair(s, find_index_position(s)));
    index_names.erase(index_names.begin() + pos);
}

// I THINK THIS IS NOT EXCEPTION SAFE!!!
void DataFrame::dropna() {
    vector<pair<string, int>> index_copy = index_names;
    for (auto index_pair : index_copy) {
        if (contains_null(index_pair.first)) {
            make_unique_if(get_column_names());
            drop_row(index_pair.first);
        }
    }
}

template <typename T>
vector<string> DataFrame::get_column_names() {
    string target;
    if (std::is_same<T, double>::value) target = "double";
    if (std::is_same<T, string>::value) target = "string";
    auto fun = [&](const pair<string, int>& x) {
        return columns[x.second]->type_name() == target ? true : false;
    };
    vector<pair<string, int>> res;
    std::copy_if(column_names.begin(), column_names.end(),
                 std::back_inserter(res), fun);
    return get_names(res);
}

template vector<string> DataFrame::get_column_names<double>();
template vector<string> DataFrame::get_column_names<string>();

void DataFrame::drop_column(const string& s) { column_names.erase(s); }

void DataFrame::sort() {
    std::sort(index_names.begin(), index_names.end(), 
            [](auto&a, auto&b) { return a.first < b.first;});
}

template <typename T>
void DataFrame::sort(const string& s) {
    vector<pair<string, int>> new_index(index_names.size());
    string target = columns[get_column_position(s)]->type_name();
    vector<int> argsort = permutation_index<T>(s);
    for (size_t i = 0; i < argsort.size(); ++i) {
        pair<string, int> tmp = index_names[argsort[i]];
        new_index[i] = tmp;
    }
    index_names = new_index;
}

template void DataFrame::sort<double>(const std::string&);
template void DataFrame::sort<std::string>(const std::string&);
