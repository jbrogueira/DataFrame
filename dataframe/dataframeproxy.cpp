#include "dataframeproxy.h"

using std::deque;
using std::string;
using std::vector;

void check_index_size(size_t width, const deque<Index::ele>& idx,
                      const char* caller) {
    if (idx.size() != width) {
        string m("An index element has " + std::to_string(width) +
                 " values, but the passed index has " +
                 std::to_string(idx.size()) + " elements, in:\n");
        throw std::invalid_argument(m + caller);
    }
}
void check_index_size(size_t width, const vector<deque<Index::ele>>& idx,
                      const char* caller) {
    for (auto const& i : idx) check_index_size(width, i, caller);
}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const string& s)
    : theDataFrame(df), colNames{s}, proxy_index(df.index) {}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& s)
    : theDataFrame(df), colNames(s), proxy_index(df.index) {}

void DataFrame::DataFrameProxy::append_index(const deque<Index::ele>& idx, 
                                             const char* caller) {
    deque<int> found_indices = theDataFrame.index.find_index_position(idx);
    if (found_indices.size() > 0)
        for (int p : found_indices) proxy_index.append_index(idx, p);
    else {
        string m("could not find the given index, in:\n");
        throw std::out_of_range(m + caller);
    }
}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const deque<Index::ele>& idx,
                                          const string& s)
    : theDataFrame(df), colNames{s}, proxy_index() {
    check_index_size(df.index.size().second, idx, __PRETTY_FUNCTION__);
    append_index(idx, __PRETTY_FUNCTION__);
}

DataFrame::DataFrameProxy::DataFrameProxy(
    DataFrame& df, const std::vector<std::deque<Index::ele>>& indices,
    const std::vector<std::string>& s)
    : theDataFrame(df), colNames(s), proxy_index() {
    check_index_size(df.index.size().second, indices, __PRETTY_FUNCTION__);
    for (auto const& idx : indices) append_index(idx, __PRETTY_FUNCTION__);
}

void DataFrame::DataFrameProxy::check_compatability(const char* parent) const {
    if (colNames.size() > 1) {
        string m("Trying to convert more than one column, in:\n");
        throw std::runtime_error(m + parent);
    }
    if (proxy_index.index_positions.size() > 1) {
        string m("Trying to convert more than one row, in:\n");
        throw std::runtime_error(m + parent);
    }
}
DataFrame::DataFrameProxy::operator double() const {
    check_compatability(__PRETTY_FUNCTION__);
    int colNumber = theDataFrame.column_names[colNames[0]];
    int pos = proxy_index.find_index_position().front();
    try {
        return theDataFrame.columns[colNumber]->get_value<double>(pos);
    } catch (std::invalid_argument& c) {
        string m("Incompatbile return type, in:\n");
        throw std::runtime_error(m + __PRETTY_FUNCTION__);
    }
}

DataFrame::DataFrameProxy::operator std::string() const {
    int colNumber = theDataFrame.column_names[colNames[0]];
    int pos = proxy_index.find_index_position().front();
    try {
        return theDataFrame.columns[colNumber]->get_value<string>(pos);
    } catch (std::invalid_argument& c) {
        string m("Incompatbile return type, in:\n");
        throw std::runtime_error(m + __PRETTY_FUNCTION__);
    }
}
