#include "lexer.hpp"
#include <iostream>
#include <sstream>

Lexer::Lexer(const std::string& src)
	: source(src), position(0), line(1), column(1){
		if(!source.empty()){
			currentChar = source[position];
		}else{
			currentChar = '\0';
		}
	}

void Lexer::advance(){
	position++;
	column++;

	if(position < source.length()){
		currentChar = source[position];
	}else{
		currentChar = '\0';
	}
}

char Lexer::peek(){
	if(position + 1 < source.length()){
		return source[position + 1];
	}
	return '\0';
}

void Lexer::skipWhitespace(){
	while(currentChar != '\0' && std::isspace(currentChar)){
		if(currentChar == '\n'){
			line++;
			column = 1;
		}
		advance();
	}
}

void Lexer::skipComment(){
	while(currentChar != '\0' && currentChar != '\n'){
		advance();
	}
	skipWhitespace();
}

bool Lexer::isKeyword(const std::string& str){
	for(const auto& keyword : keywords){
		if(keyword == str){
			return true;
		}
	}
	return false;
}

bool Lexer::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '=' || c == '<' || c == '>' || c == '!';
}

Token Lexer::readNumber() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    while (currentChar != '\0' && std::isdigit(currentChar)) {
        value += currentChar;
        advance();
    }
    
    if (currentChar == '.' && std::isdigit(peek())) {
        value += currentChar;
        advance();
        
        while (currentChar != '\0' && std::isdigit(currentChar)) {
            value += currentChar;
            advance();
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token Lexer::readIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    while (currentChar != '\0' && (std::isalnum(currentChar) || currentChar == '_')) {
        value += currentChar;
        advance();
    }
    
    if (isKeyword(value)) {
        return Token(TokenType::KEYWORD, value, startLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
}

Token Lexer::readString() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    advance();
    
    while (currentChar != '\0' && currentChar != '"') {
        if (currentChar == '\\') {
            advance();
            if (currentChar == 'n') value += '\n';
            else if (currentChar == 't') value += '\t';
            else if (currentChar == '"') value += '"';
            else value += currentChar;
        } else {
            value += currentChar;
        }
        advance();
    }
    
    if (currentChar == '"') {
        advance();
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn);
}

Token Lexer::readOperator() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    value += currentChar;
    advance();
    
    if ((value == "=" && currentChar == '=') ||
        (value == "!" && currentChar == '=') ||
        (value == "<" && currentChar == '=') ||
        (value == ">" && currentChar == '=') ||
        (value == "&" && currentChar == '&') ||
        (value == "|" && currentChar == '|')) {
        value += currentChar;
        advance();
    }
    
    return Token(TokenType::OPERATOR, value, startLine, startColumn);
}

std::vector<Token> Lexer::tokensize() {
    std::vector<Token> tokens;
    
    while (currentChar != '\0') {
	    //skip space
        if (std::isspace(currentChar)) {
            skipWhitespace();
            continue;
        }
        
        // comments
        if (currentChar == '/' && peek() == '/') {
            skipComment();
            continue;
        }
        
        // numbers
        if (std::isdigit(currentChar)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        if (std::isalpha(currentChar) || currentChar == '_') {
            tokens.push_back(readIdentifier());
            continue;
        }
        
        if (currentChar == '"') {
            tokens.push_back(readString());
            continue;
        }
        
        if (isOperator(currentChar)) {
            tokens.push_back(readOperator());
            continue;
        }
        
        switch (currentChar) {
            case '(':
                tokens.push_back(Token(TokenType::LPAREN, "(", line, column));
                advance();
                break;
            case ')':
                tokens.push_back(Token(TokenType::RPAREN, ")", line, column));
                advance();
                break;
            case '{':
                tokens.push_back(Token(TokenType::LBRACE, "{", line, column));
                advance();
                break;
            case '}':
                tokens.push_back(Token(TokenType::RBRACE, "}", line, column));
                advance();
                break;
            case ';':
                tokens.push_back(Token(TokenType::SEMICOLN, ";", line, column));
                advance();
                break;
            default:
                // unknown symbol
                std::string unknown(1, currentChar);
                tokens.push_back(Token(TokenType::UNKNOWN, unknown, line, column));
                advance();
                break;
        }
    }
    
    // add EOF
    tokens.push_back(Token(TokenType::END, "EOF", line, column));
    
    return tokens;
}

void Lexer::printTokens(const std::vector<Token>& tokens) {
    std::cout << "\n=== TOKENS ===\n\n";
    
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ", Col " << token.column << ": ";
        
        switch (token.type) {
            case TokenType::NUMBER:     std::cout << "NUMBER    "; break;
            case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::KEYWORD:    std::cout << "KEYWORD   "; break;
            case TokenType::OPERATOR:   std::cout << "OPERATOR  "; break;
            case TokenType::LPAREN:     std::cout << "LPAREN    "; break;
            case TokenType::RPAREN:     std::cout << "RPAREN    "; break;
            case TokenType::LBRACE:     std::cout << "LBRACE    "; break;
            case TokenType::RBRACE:     std::cout << "RBRACE    "; break;
            case TokenType::SEMICOLN:  std::cout << "SEMICOLN "; break;
            case TokenType::STRING:     std::cout << "STRING    "; break;
            case TokenType::COMMENT:    std::cout << "COMMENT   "; break;
            case TokenType::UNKNOWN:    std::cout << "UNKNOWN   "; break;
            case TokenType::END:        std::cout << "END       "; break;
        }
        
        std::cout << "  \"" << token.value << "\"\n";
    }
}
