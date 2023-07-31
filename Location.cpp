#include "Location.hpp"

bool operator==(const yy::position& pos0, const yy::position& pos1) {
    return pos0.line == pos1.line
        && pos0.column == pos1.column
        && *pos0.filename == *pos1.filename;
}

bool operator==(const Location& loc0, const Location& loc1) {
    return loc0.begin == loc1.begin && loc0.end == loc1.end;
}

