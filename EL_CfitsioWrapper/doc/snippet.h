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
T parseRecord(const std::string& keyword) {
    T val(1.5);
    std::cout << keyword << '\t' << val << std::endl;
    return val;
}

template<typename... Ts, long... Is>
std::tuple<Ts...> ParseRecordsImpl(const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
    return std::tuple<Ts...> { parseRecord<Ts>(keywords[Is]) ... };
}

template<class Return, typename... Ts, long... Is>
Return parseRecordsAsImpl(const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
    return { parseRecord<Ts>(keywords[Is]) ... };
}

template<typename... Ts>
std::tuple<Ts...> parseRecords(const std::vector<std::string>& keywords) {
    return ParseRecordsImpl<Ts...>(keywords, std::make_index_sequence<sizeof...(Ts)>());
}

template<class Return, typename... Ts>
Return parseRecordsAs(const std::vector<std::string>& keywords) {
    return parseRecordsAsImpl<Return, Ts...>(keywords, std::make_index_sequence<sizeof...(Ts)>());
}

int main() {
    
    auto records = parseRecords<int, float>({"int", "float"});
    std::cout << "i\t" << std::get<0>(records) << std::endl;
    std::cout << "f\t" << std::get<1>(records) << std::endl;
    
    struct Header {
        int i;
        float f;
    };
    
    auto header = parseRecordsAs<Header, int, float>({"int", "float"});
    std::cout << "i\t" << header.i << std::endl;
    std::cout << "f\t" << header.f << std::endl;
}
