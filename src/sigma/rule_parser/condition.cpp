#include <sigma/rule_parser/condition.h>
#include <functional>

namespace sigma {

    Lexer::Lexer(const std::string& str) : s(str), i(0) {}

    Token Lexer::Next()
    {
        while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
        if (i >= s.size()) return { TokenType::End, "" };

        char c = s[i];
        if (c == '(') { ++i; return { TokenType::LeftParen, "(" }; }
        if (c == ')') { ++i; return { TokenType::RightParen, ")" }; }

        size_t st = i;
        while (i < s.size() && !std::isspace((unsigned char)s[i]) && s[i] != '(' && s[i] != ')') ++i;
        std::string tk = s.substr(st, i - st);
        std::string low = tk;
        std::transform(low.begin(), low.end(), low.begin(), [](unsigned char ch) { return std::tolower(ch); });

        if (low == "and") return { TokenType::And, tk };
        if (low == "or") return { TokenType::Or, tk };
        if (low == "not") return { TokenType::Not, tk };
        if (low == "all") return { TokenType::All, tk };
        if (low == "of") return { TokenType::Of, tk };
        if (low == "them") return { TokenType::Them, tk };

        bool isnumeric = true;
        for (char ch : tk) if (!std::isdigit((unsigned char)ch)) { isnumeric = false; break; }
        if (isnumeric) return { TokenType::Num, tk };

        return { TokenType::Selection, tk };
    }

    Node::Node(Type t) : type(t) {}

    Parser::Parser(const std::string& s) : lx(s) { cur = lx.Next(); }

    void Parser::Eat(TokenType t)
    {
        if (cur.type != t) throw std::runtime_error("parse error: unexpected token '" + cur.text + "'");
        cur = lx.Next();
    }

    std::unique_ptr<Node> Parser::ParseCondition()
    {
        auto n = ParseOr();
        if (cur.type != TokenType::End)
            throw std::runtime_error("trailing token: " + cur.text);
        return n;
    }

    std::unique_ptr<Node> Parser::ParseOr()
    {
        auto left = ParseAnd();
        while (cur.type == TokenType::Or) {
            Eat(TokenType::Or);
            auto right = ParseAnd();
            auto p = std::make_unique<Node>(Node::Type::Or);
            p->children.push_back(std::move(left));
            p->children.push_back(std::move(right));
            left = std::move(p);
        }
        return left;
    }

    std::unique_ptr<Node> Parser::ParseAnd()
    {
        auto left = ParseUnary();
        while (cur.type == TokenType::And) {
            Eat(TokenType::And);
            auto right = ParseUnary();
            auto p = std::make_unique<Node>(Node::Type::And);
            p->children.push_back(std::move(left));
            p->children.push_back(std::move(right));
            left = std::move(p);
        }
        return left;
    }

    std::unique_ptr<Node> Parser::ParseUnary()
    {
        if (cur.type == TokenType::Not) {
            Eat(TokenType::Not);
            auto p = std::make_unique<Node>(Node::Type::Not);
            p->children.push_back(ParseUnary());
            return p;
        }

        if (cur.type == TokenType::All) {
            Eat(TokenType::All);
            Eat(TokenType::Of);
            auto p = std::make_unique<Node>(Node::Type::AllOf);
            if (cur.type == TokenType::Selection) {
                p->text = cur.text;
                Eat(TokenType::Selection);
                return p;
            }
            else if (cur.type == TokenType::Them) {
                p->text = "*";
                Eat(TokenType::Them);
                return p;
            }
            throw std::runtime_error("expected selector after 'all of'");
        }

        if (cur.type == TokenType::Num && cur.text == "1") {
            Eat(TokenType::Num);
            Eat(TokenType::Of);
            auto p = std::make_unique<Node>(Node::Type::OneOf);
            if (cur.type == TokenType::Selection) {
                p->text = cur.text;
                Eat(TokenType::Selection);
                return p;
            }
            else if (cur.type == TokenType::Them) {
                p->text = "*";
                Eat(TokenType::Them);
                return p;
            }
            throw std::runtime_error("expected selector after '1 of'");
        }

        return ParsePrimary();
    }

    std::unique_ptr<Node> Parser::ParsePrimary()
    {
        if (cur.type == TokenType::LeftParen) {
            Eat(TokenType::LeftParen);
            auto p = ParseOr();
            Eat(TokenType::RightParen);
            return p;
        }

        if (cur.type == TokenType::Selection) {
            auto p = std::make_unique<Node>(Node::Type::Selection);
            p->text = cur.text;
            Eat(TokenType::Selection);
            return p;
        }

        throw std::runtime_error("unexpected token in primary: " + cur.text);
    }

    namespace {
        SelectionsView ExpandPattern(const std::string& pattern, SelectionsView selections)
        {
            if (pattern.empty()) return {};
            SelectionsView out;
            for (auto& selection : selections) {
                if (utils::ConditionPatternMatch(pattern.c_str(), selection->GetName().c_str())) {
                    out.push_back(selection);
                }
            }
            return out;
        }

        bool MatchNodeCached(NodeView node, SelectionsView selections, std::unordered_map<NodeView, bool>& selectionCache,
            const std::function<bool(SelectionsView)>& matcher)
        {
            auto cached = selectionCache.find(node);
            if (cached != selectionCache.end()) return cached->second;

            bool result = matcher(selections);
            selectionCache[node] = result;
            return result;
        }

        bool EvaluateNode(NodeView node, SelectionsView selections, const Event& event,std::unordered_map<NodeView, bool>& selectionCache)
        {
            switch (node->type)
            {
                case Node::Type::True:
                    return true;

                case Node::Type::Selection:
                {
                    return MatchNodeCached(node, selections, selectionCache,
                        [&node , &event](SelectionsView candidates)
                        {
                            return std::any_of(candidates.begin(), candidates.end(),
                                [&node, &event](SelectionView s){ return s->GetName() == node->text && s->Match(event);});
                        });
                }
                case Node::Type::OneOf:
                {
                    auto expanded = ExpandPattern(node->text, selections);
                    return MatchNodeCached(node, expanded, selectionCache,
                        [&event](SelectionsView candidates)
                        {
                            if (candidates.empty()) return false;
                            return std::any_of(candidates.begin(), candidates.end(),
                                [&event](SelectionView s) { return s->Match(event); });
                        });

                }
                case Node::Type::AllOf: 
                {
                    auto expanded = ExpandPattern(node->text, selections);
                    return MatchNodeCached(node, expanded, selectionCache,
                        [&event](SelectionsView candidates)
                        {
                            if (candidates.empty()) return false;
                            return std::all_of(candidates.begin(), candidates.end(),
                                [&event](SelectionView s) { return s->Match(event); });
                        });
                }

                case Node::Type::Not:
                    return !EvaluateNode(node->children[0].get(), selections, event, selectionCache);

                case Node::Type::And:
                    return EvaluateNode(node->children[0].get(), selections, event, selectionCache)
                        && EvaluateNode(node->children[1].get(), selections, event, selectionCache);

                case Node::Type::Or:
                    return EvaluateNode(node->children[0].get(), selections, event, selectionCache)
                        || EvaluateNode(node->children[1].get(), selections, event, selectionCache);

            }

            return false;
        }
    } // namespace

    Condition::Condition(const std::string& condition)
    {
        Parser parser(condition);
        m_ast = parser.ParseCondition();
    }

    bool Condition::Evaluate(const Selections& sels, const Event& ev) const
    {
        std::unordered_map<NodeView, bool> selectionCache;
        SelectionsView view;
        view.reserve(sels.size());
        std::transform(sels.begin(), sels.end(), std::back_inserter(view),
            [](const SelectionPtr& s) { return s.get(); });
        return EvaluateNode(m_ast.get(), view, ev, selectionCache);
    }

} // sigma
