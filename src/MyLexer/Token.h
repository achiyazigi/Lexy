#include <stdlib.h>
#include <string>

namespace lxr
{
    enum class ETokenKind
    {
        End,
        Prepeocessor,
        Symbol,
        Colon,
        Semicolon,
        OpenParen,
        CloseParen,
        OpenCurly,
        CloseCurly,
        OpenSquare,
        CloseSquare,
        Number,
        Assignment,
        CondEq,
        CondNeq,
        OpNot,
        OpDiv,
        KeyWord,
        ControlKeyWord,
        String,
        Char,
        Comma,
        CommentSingleLine,
        CommentMultiline,
        Invalid
    };

    struct Token
    {

        size_t m_nBegin;
        size_t m_nLength;
        ETokenKind m_eKind = ETokenKind::Invalid;
    };
}