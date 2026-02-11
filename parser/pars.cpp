#include "parser.hpp"
#include <stdexcept>
#include <cstdlib>

namespace parse {

void Parser::consume(lex::TokenType expected){
    if(currentToken.type==expected) currentToken=lexer.next();
    else throw std::runtime_error("Unexpected token: "+currentToken.value);
}

std::unique_ptr<ast::Node> Parser::parseStatement(){
    if(currentToken.type==lex::TokenType::IDENTIFIER){
        std::string name=currentToken.value;
        consume(lex::TokenType::IDENTIFIER);
        if(currentToken.type==lex::TokenType::COLON_ASSIGN){
            consume(lex::TokenType::COLON_ASSIGN);
            auto expr=parseExpression();
            return std::make_unique<ast::Assignment>(name,std::move(expr));
        } else throw std::runtime_error("Expected := after identifier");
    }
    throw std::runtime_error("Invalid statement starting with: "+currentToken.value);
}

std::unique_ptr<ast::Node> Parser::parseExpression(){
    auto left=parseTerm();
    while(currentToken.type==lex::TokenType::PLUS || currentToken.type==lex::TokenType::MINUS){
        std::string op=currentToken.value;
        consume(currentToken.type);
        auto right=parseTerm();
        left=std::make_unique<ast::BinaryOp>(std::move(left),op,std::move(right));
    }
    return left;
}

std::unique_ptr<ast::Node> Parser::parseTerm(){
    auto left=parseFactor();
    while(currentToken.type==lex::TokenType::MUL || currentToken.type==lex::TokenType::DIV){
        std::string op=currentToken.value;
        consume(currentToken.type);
        auto right=parseFactor();
        left=std::make_unique<ast::BinaryOp>(std::move(left),op,std::move(right));
    }
    return left;
}

std::unique_ptr<ast::Node> Parser::parseFactor(){
    std::unique_ptr<ast::Node> left;

    if(currentToken.type==lex::TokenType::INTEGER){
        left=std::make_unique<ast::Number>(std::stod(currentToken.value));
        consume(lex::TokenType::INTEGER);
    } else if(currentToken.type==lex::TokenType::REAL){
        left=std::make_unique<ast::Number>(std::stod(currentToken.value));
        consume(lex::TokenType::REAL);
    } else if(currentToken.type==lex::TokenType::IDENTIFIER){
        left=std::make_unique<ast::Identifier>(currentToken.value);
        consume(lex::TokenType::IDENTIFIER);
    } else if(currentToken.type==lex::TokenType::LPAREN){
        consume(lex::TokenType::LPAREN);
        left=parseExpression();
        consume(lex::TokenType::RPAREN);
    } else throw std::runtime_error("Unexpected token in factor: "+currentToken.value);

    while(currentToken.type==lex::TokenType::POW){
        std::string op=currentToken.value;
        consume(lex::TokenType::POW);
        auto right=parseFactor();
        left=std::make_unique<ast::BinaryOp>(std::move(left),op,std::move(right));
    }

    return left;
}

} // namespace parse

