#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <algorithm>
#include <string>
#include <variant>
#include <vector>

class DataFrame;
class Column {
   public:
    friend std::ostream& operator<<(std::ostream&, const Column&);
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, const std::string&);
    friend class DataFrame;
    friend void append_missing_cols(DataFrame&, const DataFrame&);

    Column();
    Column(const Column&, int);
    Column(const Column&, const std::vector<int>&);
    Column& operator+=(const Column& rhs);
    Column& add_other_column(const Column&,
                             const std::vector<std::pair<int, int>>&);
    template <typename T>
    void add_elements(std::vector<T>*, const std::vector<T>&,
                      const std::vector<std::pair<int, int>>&);
    template <class T>
    Column(const std::vector<T>& t) {
        col = t;
    }
    template <class T>
    void push_back(const T);
    const int size();
    template <class V>
    V operator[](int);
    template <typename V>
    typename std::vector<V>::iterator begin() {
        if (std::holds_alternative<std::vector<V>>(col))
            return std::get<std::vector<V>>(col).begin();
        else
            throw std::invalid_argument("not in here");
    }
    template <typename V>
    typename std::vector<V>::iterator end() {
        if (std::holds_alternative<std::vector<V>>(col))
            return std::get<std::vector<V>>(col).end();
        else
            throw std::invalid_argument("not in here");
    }
    void append_string(std::string&, int pos);
    void push_back_nan();

   private:
    std::variant<std::vector<double>, std::vector<std::string>> col;
    void replace_nan();
    void replace_nan(int);
};

std::ostream& operator<<(std::ostream&, const Column&);
Column operator+(const Column&, double d);
Column operator+(const Column&, const std::string& s);
#endif
