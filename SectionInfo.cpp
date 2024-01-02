#include "SectionInfo.hpp"

SectionInfo::SectionInfo() {}

SectionInfo::SectionInfo(
    std::string name,
    bool writable,
    std::int64_t start,
    std::int64_t end
) : name{name}, writable{writable}, start{start}, end{end} {}

