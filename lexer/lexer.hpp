#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <cctype>

//TOKEN types
enum class TokenType{
	NUMBER,
	IDENTIFIER,
	KEYWORD,
	OPERATOR,
	LPAREN, //    (
	RPAREN, //     )
	LBRACE, //    {
	RBRACE, //    }
	SEMICOLN, //  ;
	STRING, 
	COMMENT,
	COMMA,
	UNKNOWN,
	END,
};

struct Token{
	TokenType type;
	std::string value;
	int line;
	int column;

	Token() : type(TokenType::UNKNOWN), value(""), line(0), column(0) {}
	Token(TokenType t, const std::string& v, int l, int c)
		:type(t), value(v), line(l), column(c) {}
};

class Lexer{
private:
	std::string source; // source code  (исходный код)
	size_t position;
	int line;
	int column;
	char currentChar;

	const std::vector<std::string> keywords = {
		"if", "else", "while", "for", "return",
		"int", "float", "string", "bool", "true", "false"
	};

	void advance();   // go to next symbol
	void skipWhitespace();  // skip space
	void skipComment();
	char peek();  //look next symbol
	
	Token readNumber();
	Token readIdentifier();
	Token readString();
	Token readOperator();

	bool isKeyword(const std::string& str);
	bool isOperator(char c);
public:
	Lexer(const std::string& src);
	std::vector<Token> tokensize();
	void printTokens(const std::vector<Token>& tokens);
			
};
#endif
