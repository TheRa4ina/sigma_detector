#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <sigma/rule_parser/utilities.h>
#include <sigma/rule_parser/selection.h>

namespace sigma {

    enum class TokenType {
        End, And, Or, Not, LeftParen, RightParen,
        Selection, Num, All, Of, Them
    };

    struct Token {
        TokenType type;
        std::string text;
    };

    struct Lexer {
        std::string s;
        size_t i = 0;
        Lexer(const std::string& str);
        Token Next();
    };

    struct Node {
        enum class Type { True, Selection, Not, And, Or, OneOf, AllOf } type;
        std::string text;
        std::vector<std::unique_ptr<Node>> children;

        Node(Type t = Type::True);
    };

    using NodeView = const Node*;

    struct Parser {
        Lexer lx;
        Token cur;

        Parser(const std::string& s);
        void Eat(TokenType t);
        std::unique_ptr<Node> ParseCondition();
        std::unique_ptr<Node> ParseOr();
        std::unique_ptr<Node> ParseAnd();
        std::unique_ptr<Node> ParseUnary();
        std::unique_ptr<Node> ParsePrimary();
    };
    using SelectionView = const ISelection*;
    using SelectionsView = std::vector<SelectionView>;

    class Condition {
    public:
        Condition(const std::string& condition);
        bool Evaluate(const Selections& sels, const Event& ev) const;
    private:
        std::unique_ptr<Node> m_ast;
    };

} // namespace sigma
