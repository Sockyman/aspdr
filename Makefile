
TARGET := aspdr
BUILD_DIR := build

SRCS := parser.cpp scanner.cpp main.cpp Driver.cpp Expression.cpp \
	Statement.cpp Assembler.cpp Identifier.cpp Error.cpp Location.cpp \
	Section.cpp SectionInfo.cpp InstructionStatement.cpp stringliteral.cpp \
	Context.cpp ErrorHandler.cpp DataElement.cpp Frame.cpp Macro.cpp Options.cpp

OBJECTS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

CXXFLAGS := -std=c++20 -g -c -MD -MP -Wall -pedantic -O0
LDFLAGS := -lspdr-firmware

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l

parser.cpp: parser.yy
	bison -o parser.cpp --header=parser.hpp parser.yy

parser.hpp: parser.yy
location.hh: parser.yy

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm scanner.cpp parser.cpp parser.hpp location.hh

.PHONY: install
install:
	cp $(BUILD_DIR)/$(TARGET) $(HOME)/.local/bin/$(TARGET)


-include $(DEPS)
