#ifndef MACRO_HPP
#define MACRO_HPP

#include <SpdrFirmware/Mode.hpp>
#include <variant>
#include <compare>

class Macro {
public:
    std::string name;
    //std::vector<std::string> parameterNames;
    
    Macro(std::string name);

    auto operator<=>(const Macro& macro) const = default;
    bool operator==(const Macro& macro) const = default;
};

#endif

