#ifndef FRAME_HPP
#define FRAME_HPP

#include <string>
#include <vector>
#include <optional>
#include <functional>

class Frame {
public:
    enum class Type {
        Macro,
        Loop,
        Index,
    };

    class TypeInfo {
    public:
        std::optional<std::string> prefix;
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

    std::string getIdent(std::function<bool(const Frame::TypeInfo&)> func);
public:
    FrameStack();

    void push(Frame frame);
    void pop();

    std::string getLocalIdent();
    std::string getMacroIdent();

    bool stateMutation();
};

#endif

