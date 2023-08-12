#ifndef SECTIONINFO_HPP
#define SECTIONINFO_HPP

#include <cstdint>
#include <string>

class SectionInfo {
private:
public:
    std::string name;

    bool writable;
    std::int64_t start;

    SectionInfo();
    SectionInfo(std::string name, bool writable, std::int64_t start);
};

#endif

