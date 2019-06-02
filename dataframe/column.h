#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <exception>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;

class Column {
   private:
    std::variant<std::vector<double>, std::vector<std::string>> col;
    std::string type_name;

   public:
    typedef std::vector<std::string> svec;
    typedef std::vector<double> dvec;
    friend std::ostream& operator<<(std::ostream&, const Column&);
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, std::string);
    Column();
    Column& operator+=(const Column& rhs);
    template <class T> Column(const std::vector<T>& t) {
        col = t;
        if (std::is_same<T, std::string>::value)
            type_name = "string";
        else if (std::is_same<T, double>::value)
            type_name = "double";
        else
            throw std::invalid_argument("Not a matching type");
    }
    std::string get_type() const {return type_name;}
    template <class T> void push_back(const T);
    int size();
    template <class V> V operator[](int pos) {
        if (std::holds_alternative<vector<V>>(col))
            return std::get<vector<V>>(col)[pos];
        else
            throw std::invalid_argument("not in here");
    }
    template <typename V> typename std::vector<V>::iterator begin() {
        if (std::holds_alternative<vector<V>>(col))
            return std::get<vector<V>>(col).begin();
        else
            throw std::invalid_argument("not in here");
    }
    template <typename V> typename std::vector<V>::iterator end() {
        if (std::holds_alternative<vector<V>>(col))
            return std::get<vector<V>>(col).end();
        else
            throw std::invalid_argument("not in here");
    }
    void append_string(std::string&, int pos);
};

std::ostream& operator<<(std::ostream&, const Column&);
Column operator+(const Column&, double d);
Column operator+(const Column&, std::string s);
//template <typename T>
//Column operator+(const Column& c, T s) {
    ////Column new_col;
    //if (const vector<T>* dvec = std::get_if<vector<T>>(&c.col)) {
        //vector<T> lhs = std::vector<T>(dvec->size(), s);
        //std::transform(dvec->begin(), dvec->end(), lhs.begin(),
                       //lhs.begin(), std::plus<T>());
        //return Column(lhs);
        ////new_col = Column(lhs);
    //}else {
        //throw std::invalid_argument("Cant combine a non-double with a double");
    //}
    ////return new_col;
//}
#endif
