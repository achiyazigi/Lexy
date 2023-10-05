/*
    This lexer meant for highliting purposes only.
    It doesn't throw errors.
    In case of invalid token, ETokenKind::Invalid would be returned.
    The architecture of this lexer is inspired by tsoding which implemented in C.
    This lexer is the C++ version of tsoding's lexer for his editor: "ded"
*/

#include <string>
#include "Token.h"
#include <unordered_set>

namespace lxr
{
    class MyLexer
    {
    public:
        MyLexer(const std::string &a_strContent);
        Token get();

    private:
        char consume();
        char peek();
        const char *ptr();
        void trimLeft();
        bool isSymbolStart();
        bool isSymbol();
        bool isNumber();

        const std::string m_strContent;
        size_t m_nCursor;
    };
}