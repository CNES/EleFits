/**
 * Parse records as user-defined structs.
 * Requires C++14 but could be adapter for C++11.
 */

#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>


template<typename T>
T parse_record(const std::string& keyword) {
    T val(1.5);
    std::cout << keyword << '\t' << val << std::endl;
    return val;
}

template<typename... Ts, long... Is>
std::tuple<Ts...> _parse_records(const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
    return std::tuple<Ts...> { parse_record<Ts>(keywords[Is]) ... };
}

template<class Return, typename... Ts, long... Is>
Return _parse_records_as(const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
    return { parse_record<Ts>(keywords[Is]) ... };
}

template<typename... Ts>
std::tuple<Ts...> parse_records(const std::vector<std::string>& keywords) {
    return _parse_records<Ts...>(keywords, std::make_index_sequence<sizeof...(Ts)>());
}

template<class Return, typename... Ts>
Return parse_records_as(const std::vector<std::string>& keywords) {
    return _parse_records_as<Return, Ts...>(keywords, std::make_index_sequence<sizeof...(Ts)>());
}

int main() {
    
    auto records = parse_records<int, float>({"int", "float"});
    std::cout << "i\t" << std::get<0>(records) << std::endl;
    std::cout << "f\t" << std::get<1>(records) << std::endl;
    
    struct Header {
        int i;
        float f;
    };
    
    auto header = parse_records_as<Header, int, float>({"int", "float"});
    std::cout << "i\t" << header.i << std::endl;
    std::cout << "f\t" << header.f << std::endl;
}
