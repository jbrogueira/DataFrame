#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <cxxabi.h>
#include <algorithm>
#include <deque>
#include <string>
#include <variant>
#include <vector>

class DataFrame;
class Column {
   public:
    template <typename T>
    friend Column operator+(const Column&, const T&);
    friend class DataFrame;
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    template <typename T>
    friend DataFrame operator<(const DataFrame&, T);
    template <typename T>
    friend DataFrame operator>(const DataFrame&, T);

    Column() : col(){};
    /**
     * @brief Copy constructor for a new column, given an existing colum and a
     * vector<int> indicating which position shall be copied
     */
    Column(const Column&, const std::deque<int>&);
    /**
     * @brief constructs a column form a existing vector.
     * The vector must contains types of either double, string or boolean
     */
    template <class T>
    explicit Column(const std::vector<T>& t) : col(t) {}
    /**
     * @brief adds two columns with each other, in order to c
     */
    Column& plus(const Column&, const std::deque<std::pair<int, int>>& c);
    /**
     * Pushes a string value into the column. The function is used in the csv
     * reader where a string is read and one needs to try to convert the value
     * and append the column.
     */
    void convert_and_push_back(const std::string&);
    /**
     * @An element of type T is appended to the end of the column
     */
    template <class T>
    void push_back(const T);
    /**
     * @brief Returns the length of the column
     */
    size_t size();
    size_t size() const;
    /**
     * @breif Returns the type of the stored data as a string
     */
    std::string type_name();
    template <class T>
    T& get_value(int i);
    //template <typename T>
    //void push_back_nan();
    /*
     * The column is append with an na value
     */
    void push_back_nan();
    /**
     * Convert the value of the column at positon i to a string. The function
     * is used for printing a dataframe
     */
    std::string to_string(int) const;
    void convert_bool_to_double();
    Column convert_bool_to_double(const Column&);
    void copy_row(int);

   private:
    std::variant<std::vector<double>, std::vector<std::string>,
                 std::vector<bool>>
        col;
    /*
     * @brief copies the vector in for the constructor of the column
     */
    template <typename T>
    void copy_vector(const std::vector<T>*, const std::deque<int>&);
    void replace_nan();
    void replace_nan(int);
    void is_null(std::vector<int>&);
    bool is_null(size_t);
    /*
     * @brief adds the two vectors to each other in otder of c. The function is
     * used when addding two column
     */ 
    template <typename T>
    void add_elements(std::vector<T>*, const std::vector<T>&,
                      const std::deque<std::pair<int, int>>&c);

    void add_to_double(const Column&, const std::deque<std::pair<int, int>>&);
    void add_to_string(const Column&, const std::deque<std::pair<int, int>>&);
    template <typename T>
    void is_smaller_than(T, const std::string&);
    template <typename T>
    void is_greater_than(T, const std::string&);
};

template <class T>
T& Column::get_value(int i) {
    if (std::holds_alternative<std::vector<T>>(col))
        return std::get<std::vector<T>>(col)[i];
    else {
        std::string s = "incompatible template type in\n: ";
        throw std::invalid_argument(s + __PRETTY_FUNCTION__);
    }
}

template <class T>
void Column::push_back(const T t) {
    if (std::holds_alternative<std::vector<T>>(col))
        std::get<std::vector<T>>(col).push_back(t);
    else {
        std::string s = "cannot push type into column, in\n: ";
        throw std::invalid_argument(s + __PRETTY_FUNCTION__);
    }
}

template <typename T>
void invalid_template_argument(const std::string& s, const char* fun) {
    std::string name(abi::__cxa_demangle(typeid(T).name(), 0, 0, 0));
    std::string m("Column " + s + " different type then " + name + "in:\n");
    std::string m2(m + __PRETTY_FUNCTION__ + "\ncalled from:\n:");
    throw std::invalid_argument(m2 + fun);
}

template <typename T>
void invalid_template_argument(const char* fun) {
    std::string name(abi::__cxa_demangle(typeid(T).name(), 0, 0, 0));
    std::string m("Column type different type then " + name + "in:\n");
    std::string m2(m + __PRETTY_FUNCTION__ + "\ncalled from:\n:");
    throw std::invalid_argument(m2 + fun);
}

template <typename T>
void Column::is_smaller_than(T t, const std::string& s) {
    std::vector<bool> r(size());
    if (std::vector<T>* d = std::get_if<std::vector<T>>(&col))
        std::transform(d->begin(), d->end(), r.begin(),
                       [&](T& x) { return x < t; });
    else
        invalid_template_argument<T>(s, __PRETTY_FUNCTION__);
    col = r;
}

template <typename T>
void Column::is_greater_than(T t, const std::string& s) {
    std::vector<bool> r(size());
    if (std::vector<T>* d = std::get_if<std::vector<T>>(&col))
        std::transform(d->begin(), d->end(), r.begin(),
                       [&](T& x) { return x > t; });
    else {
        invalid_template_argument<T>(s, __PRETTY_FUNCTION__);
    }
    col = r;
}

template <typename T>
std::vector<T> transform_column(const std::vector<T>* v, const T& t) {
    std::vector<T> res = std::vector<T>(v->size(), t);
    transform(v->begin(), v->end(), res.begin(), res.begin(), std::plus<T>());
    return res;
}

template <typename T>
Column operator+(const Column& c, const T& t) {
    if constexpr ((std::is_same<T, double>::value |
                   std::is_same<T, std::string>::value)) {
        if (const std::vector<T>* vec = std::get_if<std::vector<T>>(&c.col))
            return Column(transform_column(vec, t));
        else
            invalid_template_argument<T>(__PRETTY_FUNCTION__);
    } else
        return invalid_template_argument<T>(__PRETTY_FUNCTION__);
}
#endif
