#include "MyLexer.h"

using namespace lxr;

// "using" and "namespace" are here... :^p
const std::unordered_set<std::string> vKeyWords(
    {"using", "auto", "break", "case", "char",
     "const", "continue", "default", "do",
     "double", "else", "enum", "extern",
     "float", "for", "goto", "if",
     "int", "long", "register", "return",
     "short", "signed", "sizeof", "static",
     "struct", "switch", "typedef", "union",
     "unsigned", "void", "volatile", "while", "namespace"});

const std::unordered_set<std::string> vControlKeyWords(
    {"using", "break", "case", "continue", "default", "do",
     "else", "extern", "for", "goto", "if", "return",
     "switch", "while"});

MyLexer::MyLexer(const std::string &a_strContent)
    : m_strContent(a_strContent),
      m_nCursor(0)
{
}

char MyLexer::peek()
{
    assert(m_nCursor < m_strContent.size());
    return m_strContent[m_nCursor];
}

char MyLexer::consume()
{
    char c = peek();
    ++m_nCursor;
    return c;
}

const char *MyLexer::ptr()
{
    return m_strContent.data() + m_nCursor;
}

void MyLexer::trimLeft()
{
    while (m_nCursor < m_strContent.size() && isspace(peek()))
    {
        consume();
    }
}

bool MyLexer::isSymbolStart()
{
    return isalpha(peek()) || peek() == '_';
}
bool MyLexer::isSymbol()
{
    return isalnum(peek()) || peek() == '_';
}
bool MyLexer::isNumber()
{
    return isnumber(peek());
}

Token MyLexer::get()
{
    trimLeft();
    Token t{m_nCursor, 1};
    if (m_nCursor == m_strContent.size())
    {
        t.m_eKind = ETokenKind::End;
        return t;
    }

    switch (peek())
    {
    case ';':
        t.m_eKind = ETokenKind::Semicolon;
        consume();
        return t;

    case ':':
        t.m_eKind = ETokenKind::Colon;
        consume();
        return t;

    case '#':
        consume();
        while (m_nCursor < m_strContent.size() && peek() != '\n')
        {
            if (consume() == '\\')
            {
                trimLeft();
            }
        }
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Prepeocessor;
        return t;
    case '(':
        consume();
        t.m_eKind = ETokenKind::OpenParen;
        return t;
    case ')':
        consume();
        t.m_eKind = ETokenKind::CloseParen;
        return t;
    case '{':
        consume();
        t.m_eKind = ETokenKind::OpenCurly;
        return t;
    case '}':
        consume();
        t.m_eKind = ETokenKind::CloseCurly;
        return t;
    case '[':
        consume();
        t.m_eKind = ETokenKind::OpenSquare;
        return t;
    case ']':
        consume();
        t.m_eKind = ETokenKind::CloseSquare;
        return t;
    case '=':
        consume();
        if (peek() == '=')
        {
            consume();
            t.m_nLength = 2;
            t.m_eKind = ETokenKind::CondEq;
            return t;
        }
        t.m_eKind = ETokenKind::Assignment;
        return t;
    case '!':
        consume();
        if (peek() == '=')
        {
            consume();
            t.m_nLength = 2;
            t.m_eKind = ETokenKind::CondNeq;
            return t;
        }
        t.m_eKind = ETokenKind::OpNot;
        return t;
    case '\"':
        consume();
        while (m_nCursor < m_strContent.size() && peek() != '\n')
        {
            if (consume() == '\"')
            {
                t.m_nLength = m_nCursor - t.m_nBegin;
                t.m_eKind = ETokenKind::String;
                return t;
            }
        }
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Invalid;
        return t;
    case '\'':
        consume();
        if (m_nCursor < m_strContent.size() - 1)
        {
            consume(); // the character between \'
            if (consume() == '\'')
            {
                t.m_nLength = m_nCursor - t.m_nBegin;
                t.m_eKind = ETokenKind::Char;
                return t;
            }
        }
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Invalid;
        return t;
    case '/':
        consume();
        switch (peek())
        {
        case '/':
            consume();
            while (m_nCursor < m_strContent.size() && peek() != '\n')
            {
                consume();
            }
            t.m_nLength = m_nCursor - t.m_nBegin;
            t.m_eKind = ETokenKind::CommentSingleLine;
            return t;
        case '*':
            consume();
            while (m_nCursor < m_strContent.size())
            {
                if (consume() == '*')
                {
                    if (m_nCursor < m_strContent.size() && consume() == '/')
                    {
                        t.m_nLength = m_nCursor - t.m_nBegin;
                        t.m_eKind = ETokenKind::CommentMultiline;
                        return t;
                    }
                }
            }
            t.m_nLength = m_nCursor - t.m_nBegin;
            t.m_eKind = ETokenKind::Invalid;
            return t;
        default:
            t.m_nLength = m_nCursor - t.m_nBegin;
            t.m_eKind = ETokenKind::OpDiv;
            return t;
        }
    case ',':
        consume();
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Comma;
        return t;
    }

    if (isSymbolStart())
    {
        while (m_nCursor < m_strContent.size() && isSymbol())
        {
            consume();
        }
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Symbol;
        std::string strSymbol = m_strContent.substr(t.m_nBegin, t.m_nLength);
        auto it = vKeyWords.find(strSymbol);
        if (it != vKeyWords.end())
        {
            t.m_eKind = ETokenKind::KeyWord;
            it = vControlKeyWords.find(strSymbol);
            if (it != vControlKeyWords.end())
            {
                t.m_eKind = ETokenKind::ControlKeyWord;
            }
        }
        return t;
    }

    if (isNumber())
    {
        bool bHexPetential = peek() == '0';
        consume();

        if (peek() == 'x' && bHexPetential)
        {
            consume();
            while (ishexnumber(peek()))
            {
                consume();
            }
            t.m_nLength = m_nCursor - t.m_nBegin;
            t.m_eKind = ETokenKind::Number;
            return t;
        }
        bool floating = false;
        if (peek() == '.')
        {
            consume();
            floating = true;
        }
        while (isNumber())
        {
            consume();
            if (peek() == 'e')
            {
                consume();
                if (peek() != '+' && peek() != '-')
                { // invalid
                    t.m_nLength = m_nCursor - t.m_nBegin;
                    return t;
                }
                consume();
            }
            else if (peek() == '.')
            {
                consume();
                if (floating)
                { // invalid
                    t.m_nLength = m_nCursor - t.m_nBegin;
                    return t;
                }
                floating = true;
            }
        }
        t.m_nLength = m_nCursor - t.m_nBegin;
        t.m_eKind = ETokenKind::Number;
        return t;
    }

    consume();
    return t; // invalid
}