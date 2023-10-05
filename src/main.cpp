#include <iostream>
#include <fstream>
#include <sstream>
#include "MyLexer/MyLexer.h"
#include "include/magic_enum.hpp"
#include <unordered_map>

#define ESC_COLOR ("\033[0m")
#define SELECTION_BACKGROUND ("\033[48;2;60;60;60m") // for future editor!

/*
Since std::format inroduced only since C++20, I took this util function from:
https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
*/
template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0)
    {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::string rgbToAnsi(uint8_t r, uint8_t g, uint8_t b, bool foreground = true)
{
    uint8_t nMode = foreground ? 38 : 48;
    return string_format("\033[%d;2;%d;%d;%dm", nMode, r, g, b);
}

size_t colorToken(std::string &strCode, const lxr::Token &token, const std::string &color, size_t offset)
{
    strCode.insert(token.m_nBegin + offset, color);
    offset += color.size();
    strCode.insert(token.m_nBegin + token.m_nLength + offset, ESC_COLOR);
    offset += 4;
    return offset;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " <source_path>" << std::endl;
    }
    std::ifstream codeFile(argv[1]);
    std::stringstream buffer;
    buffer << codeFile.rdbuf();
    std::string strCode = buffer.str();

    std::string strDup(strCode);

    lxr::MyLexer l(
        strCode);

    lxr::Token t;
    size_t nOffset = 0;

    // Very close to vscode c theme
    std::unordered_map<lxr::ETokenKind, std::string> mTokenToColor{
        {lxr::ETokenKind::Prepeocessor, rgbToAnsi(197, 134, 192)},

        {lxr::ETokenKind::Assignment, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::Colon, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::Semicolon, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::Comma, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::CondNeq, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::CondEq, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::OpNot, rgbToAnsi(204, 204, 204)},
        {lxr::ETokenKind::OpDiv, rgbToAnsi(204, 204, 204)},

        {lxr::ETokenKind::Invalid, rgbToAnsi(244, 71, 71)},

        {lxr::ETokenKind::Number, rgbToAnsi(181, 206, 168)},

        {lxr::ETokenKind::OpenCurly, rgbToAnsi(218, 112, 214)},
        {lxr::ETokenKind::CloseCurly, rgbToAnsi(218, 112, 214)},

        {lxr::ETokenKind::OpenParen, rgbToAnsi(255, 215, 0)},
        {lxr::ETokenKind::CloseParen, rgbToAnsi(255, 215, 0)},

        {lxr::ETokenKind::OpenSquare, rgbToAnsi(23, 158, 255)},
        {lxr::ETokenKind::CloseSquare, rgbToAnsi(23, 158, 255)},

        {lxr::ETokenKind::Symbol, rgbToAnsi(156, 220, 254)},

        {lxr::ETokenKind::String, rgbToAnsi(206, 145, 120)},
        {lxr::ETokenKind::Char, rgbToAnsi(206, 145, 120)},

        {lxr::ETokenKind::CommentSingleLine, rgbToAnsi(107, 153, 85)},
        {lxr::ETokenKind::CommentMultiline, rgbToAnsi(107, 153, 85)},

        {lxr::ETokenKind::ControlKeyWord, rgbToAnsi(197, 134, 192)},

        {lxr::ETokenKind::KeyWord, rgbToAnsi(86, 156, 214)}};

    while ((t = l.get()).m_eKind != lxr::ETokenKind::End)
    {
        nOffset = colorToken(strDup, t, mTokenToColor[t.m_eKind], nOffset);
    }

    std::cout << strDup << std::endl;
}
