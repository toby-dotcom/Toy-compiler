#pragma once
#include "../lexer/lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <string>

namespace parse {

class Parser {
    lex::Lexer& lexer;
    lex::Token currentToken;

    void consume(lex::TokenType expected);
    std::unique_ptr<ast::Node> parseTerm();
    std::unique_ptr<ast::Node> parseFactor();
    std::unique_ptr<ast::Node> parseExpression();

public:
    Parser(lex::Lexer& l): lexer(l), currentToken(lexer.next()) {}
    std::unique_ptr<ast::Node> parseStatement();
};

} // namespace parse

