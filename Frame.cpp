#include "Frame.hpp"
#include "Error.hpp"
#include <map>
#include <sstream>

Frame::TypeInfo::TypeInfo(std::string prefix, bool macro, bool allowState)
    : prefix{prefix}, macro{macro}, allowState{allowState} {}

const Frame::TypeInfo& Frame::typeInfo(Frame::Type type) {
    static std::map<Frame::Type, Frame::TypeInfo> typeInfos {
        { Frame::Type::Macro, {"m", true, true} },
        { Frame::Type::Loop, {"l", false, false} },
        { Frame::Type::Index, {"i", false, true} },
    };

    return typeInfos.at(type);
}

Frame::Frame(Frame::Type type, int uniqueIndex)
    : type{type}, uniqueIndex{uniqueIndex} {}


FrameStack::FrameStack() : frames{} {}


void FrameStack::push(Frame frame) {
    this->frames.push_back(frame);
}

void FrameStack::pop() {
    if (this->frames.size() <= 0) {
        ASSEMBLER_ERROR("pop on empty frame stack");
    }
    this->frames.pop_back();
}


std::string FrameStack::getLocalIdent() {
    return this->getIdent([](const Frame::TypeInfo& info) {
        return true;
    });
}

std::string FrameStack::getMacroIdent() {
    return this->getIdent([](const Frame::TypeInfo& info) {
        return info.macro;
    });
}

bool FrameStack::stateMutation() {
    for (const auto& frame : this->frames) {
        if (!Frame::typeInfo(frame.type).allowState) {
            return false;
        }
    }
    return true;
}

