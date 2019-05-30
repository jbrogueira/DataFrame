#include "column.h"
#include <algorithm>
using std::string;
using std::vector;
template <class T>
void Column::push_back(const T t) {
    std::get<vector<T>>(col).push_back(t);
}

int Column::size() {
    if (std::holds_alternative<dvec>(col))
        return std::get<dvec>(col).size();
    else if (std::holds_alternative<svec>(col))
        return std::get<svec>(col).size();
    else
        return 0;
}

void Column::append_string(std::string& s, int pos) {
    if (vector<double>* val = std::get_if<std::vector<double>>(&col))
        s += std::to_string(val->at(pos));
    if (vector<std::string>* val = std::get_if<std::vector<std::string>>(&col))
        s += val->at(pos);
}

std::ostream& operator<<(std::ostream& os, const Column& df) {
    string output;
    if (std::holds_alternative<vector<double>>(df.col)) {
        vector<double> tmp = std::get<vector<double>>(df.col);
        for (double& j : tmp) output += (std::to_string(j) + " ");
    } else if (std::holds_alternative<vector<std::string>>(df.col)) {
        vector<string> tmp = std::get<vector<string>>(df.col);
        for (string& j : tmp) output += (j + " ");
    }
    os << output;
    return os;
}

Column& Column::operator+=(const Column& rhs) {
    if (vector<double>* dvec = std::get_if<vector<double>>(&col)) {
        const vector<double>* other = std::get_if<vector<double>>(&rhs.col);
        std::transform(other->begin(), other->end(), dvec->begin(),
                       dvec->begin(), std::plus<double>());
    } else if (vector<string>* svec = std::get_if<vector<string>>(&col)) {
        const vector<string>* other = std::get_if<vector<string>>(&rhs.col);
        std::transform(other->begin(), other->end(), svec->begin(),
                       svec->begin(), std::plus<string>());
    }
    return *this;
}
