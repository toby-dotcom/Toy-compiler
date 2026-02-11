#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <cctype>

namespace lex {

enum class TokenType {

    // literals
    INTEGER,
    REAL,
    STRING,
    IDENTIFIER,

    // operators
    PLUS, MINUS, MUL, DIV, MOD, POW,
    SHIFT_RIGHT, SHIFT_LEFT,
    GT, LT, GTE, LTE,
    ASSIGN,
    COLON_ASSIGN,

    // brackets
    LPAREN, RPAREN,
    LBRACKET, RBRACKET,
    LBRACE, RBRACE,

    END,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

class Lexer {
public:
    explicit Lexer(const std::string& src)
        : source(src), pos(0), line(1), column(1) {}

    Token next() {
        skipWhitespace();

        if (isEOF())
            return makeToken(TokenType::END, "");

        char c = current();

        if (std::isalpha(c))
            return identifier();

        if (std::isdigit(c))
            return number();

        if (c == '\'')
            return string();

        return operatorOrBracket();
    }

private:
    std::string source;
    size_t pos;
    size_t line;
    size_t column;

    // =====================
    // utility
    // =====================

    bool isEOF() const {
        return pos >= source.size();
    }

    char current() const {
        if (isEOF()) return '\0';
        return source[pos];
    }

    char peek() const {
        if (pos + 1 >= source.size()) return '\0';
        return source[pos + 1];
    }

    void advance() {
        if (current() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }

    void skipWhitespace() {
        while (!isEOF() && std::isspace(current()))
            advance();
    }

    Token makeToken(TokenType type, const std::string& value) {
        return Token{type, value, line, column};
    }

    // =====================
    // identifier
    // =====================

    Token identifier() {
        size_t startCol = column;
        std::string result;

        while (std::isalnum(current()) || current() == '_') {
            result += current();
            advance();
        }

        return Token{TokenType::IDENTIFIER, result, line, startCol};
    }

    // =====================
    // numbers
    // =====================

    Token number() {
        size_t startCol = column;
        std::string result;
        bool isReal = false;

        while (std::isdigit(current())) {
            result += current();
            advance();
        }

        // decimal part
        if (current() == '.') {
            isReal = true;
            result += '.';
            advance();

            while (std::isdigit(current())) {
                result += current();
                advance();
            }
        }

        // scientific part
        if (current() == 'e' || current() == 'E') {
            isReal = true;
            result += current();
            advance();

            if (current() == '+' || current() == '-') {
                result += current();
                advance();
            }

            while (std::isdigit(current())) {
                result += current();
                advance();
            }
        }

        if (isReal)
            return Token{TokenType::REAL, result, line, startCol};
        else
            return Token{TokenType::INTEGER, result, line, startCol};
    }

    // =====================
    // string
    // =====================

    Token string() {
        size_t startCol = column;
        advance(); // skip opening '

        std::string result;

        while (!isEOF() && current() != '\'') {
            result += current();
            advance();
        }

        if (current() == '\'')
            advance();
        else
            return Token{TokenType::INVALID, result, line, startCol};

        return Token{TokenType::STRING, result, line, startCol};
    }

    // =====================
    // operators
    // =====================

    Token operatorOrBracket() {
        size_t startCol = column;
        char c = current();
        char n = peek();
        advance();

        switch (c) {

            case '+': return Token{TokenType::PLUS, "+", line, startCol};
            case '-': return Token{TokenType::MINUS, "-", line, startCol};
            case '*': return Token{TokenType::MUL, "*", line, startCol};
            case '/': return Token{TokenType::DIV, "/", line, startCol};
            case '%': return Token{TokenType::MOD, "%", line, startCol};
            case '^': return Token{TokenType::POW, "^", line, startCol};

            case '>':
                if (n == '=') { advance(); return Token{TokenType::GTE, ">=", line, startCol}; }
                if (n == '>') { advance(); return Token{TokenType::SHIFT_RIGHT, ">>", line, startCol}; }
                return Token{TokenType::GT, ">", line, startCol};

            case '<':
                if (n == '=') { advance(); return Token{TokenType::LTE, "<=", line, startCol}; }
                if (n == '<') { advance(); return Token{TokenType::SHIFT_LEFT, "<<", line, startCol}; }
                return Token{TokenType::LT, "<", line, startCol};

            case ':':
                if (n == '=') { advance(); return Token{TokenType::COLON_ASSIGN, ":=", line, startCol}; }
                break;

            case '=':
                return Token{TokenType::ASSIGN, "=", line, startCol};

            case '(':
                return Token{TokenType::LPAREN, "(", line, startCol};
            case ')':
                return Token{TokenType::RPAREN, ")", line, startCol};
            case '[':
                return Token{TokenType::LBRACKET, "[", line, startCol};
            case ']':
                return Token{TokenType::RBRACKET, "]", line, startCol};
            case '{':
                return Token{TokenType::LBRACE, "{", line, startCol};
            case '}':
                return Token{TokenType::RBRACE, "}", line, startCol};
        }

        return Token{TokenType::INVALID, std::string(1, c), line, startCol};
    }
};

} // namespace lex

#endif

