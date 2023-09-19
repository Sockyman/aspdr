#ifndef FRAME_HPP
#define FRAME_HPP

#include <string>
#include <vector>
#include <sstream>

class Frame {
public:
    enum class Type {
        Macro,
        Loop,
        Index,
    };

    class TypeInfo {
    public:
        std::string prefix;
        bool macro;
        bool allowState;

        TypeInfo(std::string prefix, bool macro, bool allowState);
    };

    static const Frame::TypeInfo& typeInfo(Frame::Type type);

    Frame::Type type;
    int uniqueIndex;

    Frame(Frame::Type type, int uniqueIndex);
};

class FrameStack {
private:
    std::vector<Frame> frames;

    template<typename F>
    std::string getIdent(F func);
public:
    FrameStack();

    void push(Frame frame);
    void pop();

    std::string getLocalIdent();
    std::string getMacroIdent();

    bool stateMutation();
};

template<typename F>
std::string FrameStack::getIdent(F func) {
    std::stringstream ss{};

    ss << "_";
    for (auto& frame : this->frames) {
        const Frame::TypeInfo& info = Frame::typeInfo(frame.type);
        if (func(info)) {
            ss << '_' << info.prefix << frame.uniqueIndex;
        }
    }
    return ss.str();
}

#endif

