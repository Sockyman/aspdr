#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "location.hh"

using Location = yy::location;

bool operator==(const yy::position& pos0, const yy::position& pos1);
bool operator==(const Location& loc0, const Location& loc1);

#endif

