#include "parser.hpp"
#include <sstream>

Parser::Parser(const std::vector<Token>& inputTokens) 
    : tokens(inputTokens), position(0) {
    if (!tokens.empty()) {
        currentToken = tokens[position];
    }
}

void Parser::advance() {
    position++;
    if (position < tokens.size()) {
        currentToken = tokens[position];
    }
}

Token Parser::peek(int offset) {
    size_t peekPos = position + offset;
    if (peekPos < tokens.size()) {
        return tokens[peekPos];
    }
    return Token(TokenType::END, "EOF", 0, 0);
}

bool Parser::match(TokenType type) {
    return currentToken.type == type;
}

bool Parser::match(const std::string& value) {
    return currentToken.value == value;
}

void Parser::expect(TokenType type, const std::string& errorMessage) {
    if (currentToken.type != type) {
        std::cerr << "Ошибка в строке " << currentToken.line 
                  << ", позиция " << currentToken.column << ": ";
        std::cerr << errorMessage << std::endl;
        std::cerr << "Ожидался " << static_cast<int>(type) 
                  << ", получен " << static_cast<int>(currentToken.type) 
                  << " (" << currentToken.value << ")" << std::endl;
        throw std::runtime_error(errorMessage);
    }
}

void Parser::expect(const std::string& value, const std::string& errorMessage) {
    if (currentToken.value != value) {
        std::cerr << "Ошибка в строке " << currentToken.line 
                  << ", позиция " << currentToken.column << ": ";
        std::cerr << errorMessage << std::endl;
        std::cerr << "Ожидался '" << value << "', получен '" 
                  << currentToken.value << "'" << std::endl;
        throw std::runtime_error(errorMessage);
    }
}

// main methods parsing

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> program;
    
    while (currentToken.type != TokenType::END) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program.push_back(std::move(statement));
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка парсинга: " << e.what() << std::endl;
            while (currentToken.type != TokenType::END && 
                   currentToken.type != TokenType::SEMICOLON) {
                advance();
            }
            if (currentToken.type == TokenType::SEMICOLON) {
                advance();
            }
        }
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::KEYWORD) && 
        (currentToken.value == "int" || 
         currentToken.value == "float" || 
         currentToken.value == "string" || 
         currentToken.value == "bool")) {
        return parseVarDeclaration();
    }
    
    // If
    if (match(TokenType::KEYWORD) && currentToken.value == "if") {
        return parseIfStatement();
    }
    
    // While
    if (match(TokenType::KEYWORD) && currentToken.value == "while") {
        return parseWhileStatement();
    }
    
    if (match(TokenType::IDENTIFIER) && peek().type == TokenType::LPAREN) {
        return parseFunctionCall();
    }
    
    if (match(TokenType::IDENTIFIER) && peek().value == "=") {
        return parseAssignment();
    }
    
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    
    auto expr = parseExpression();
    
    if (currentToken.type == TokenType::SEMICOLON) {
        advance();
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseVarDeclaration() {
    std::string type = currentToken.value;
    advance(); 
    
    expect(TokenType::IDENTIFIER, "Ожидается имя переменной");
    std::string name = currentToken.value;
    advance();
    
    std::unique_ptr<ASTNode> initializer = nullptr;
    
    if (match("=")) {
        advance(); 
        initializer = parseExpression();
    }
    
    expect(TokenType::SEMICOLON, "Ожидается ';' после объявления переменной");
    advance();
    
    return std::make_unique<VarDeclarationNode>(type, name, std::move(initializer));
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::string name = currentToken.value;
    advance(); 
    
    expect("=", "Ожидается '=' в присваивании");
    advance(); 
    
    auto value = parseExpression();
    
    expect(TokenType::SEMICOLON, "Ожидается ';' после присваивания");
    advance();
    
    return std::make_unique<AssignmentNode>(name, std::move(value));
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    advance(); // пропускаем 'if'
    
    expect(TokenType::LPAREN, "Ожидается '(' после if");
    advance();
    
    auto condition = parseExpression();
    
    expect(TokenType::RPAREN, "Ожидается ')' после условия");
    advance();
    
    auto ifNode = std::make_unique<IfNode>(std::move(condition));
    
    if (match(TokenType::LBRACE)) {
        auto block = parseBlock();
        if (auto blockNode = dynamic_cast<IfNode*>(block.get())) {
        }
    } else {
        ifNode->thenBody.push_back(parseStatement());
    }
    
    if (match(TokenType::KEYWORD) && currentToken.value == "else") {
        advance();
        
        if (match(TokenType::LBRACE)) {
            auto block = parseBlock();
        } else {
            ifNode->elseBody.push_back(parseStatement());
        }
    }
    
    return ifNode;
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    advance(); 
    
    expect(TokenType::LPAREN, "Ожидается '(' после while");
    advance();
    
    auto condition = parseExpression();
    
    expect(TokenType::RPAREN, "Ожидается ')' после условия");
    advance();
    
    auto whileNode = std::make_unique<WhileNode>(std::move(condition));
    
    if (match(TokenType::LBRACE)) {
        auto block = parseBlock();
    } else {
        whileNode->body.push_back(parseStatement());
    }
    
    return whileNode;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    expect(TokenType::LBRACE, "Ожидается '{' в начале блока");
    advance();
    
    while (currentToken.type != TokenType::RBRACE && 
           currentToken.type != TokenType::END) {
        parseStatement();
    }
    
    expect(TokenType::RBRACE, "Ожидается '}' в конце блока");
    advance();
    
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall() {
    std::string name = currentToken.value;
    advance(); 
    
    expect(TokenType::LPAREN, "Ожидается '(' после имени функции");
    advance();
    
    auto funcCall = std::make_unique<FunctionCallNode>(name);
    
    // paring arguments 
    if (currentToken.type != TokenType::RPAREN) {
        funcCall->arguments.push_back(parseExpression());
        
        while (match(TokenType::COMMA)) {
            advance();
            funcCall->arguments.push_back(parseExpression());
        }
    }
    
    expect(TokenType::RPAREN, "Ожидается ')' после аргументов");
    advance();
    
    expect(TokenType::SEMICOLON, "Ожидается ';' после вызова функции");
    advance();
    
    return funcCall;
}

//parsing operators
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseAdditive();
    
    while (match(TokenType::OPERATOR) && 
           (currentToken.value == "==" || 
            currentToken.value == "!=" || 
            currentToken.value == "<" || 
            currentToken.value == ">" || 
            currentToken.value == "<=" || 
            currentToken.value == ">=")) {
        std::string op = currentToken.value;
        advance();
        auto right = parseAdditive();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    auto left = parseMultiplicative();
    
    while (match(TokenType::OPERATOR) && 
           (currentToken.value == "+" || currentToken.value == "-")) {
        std::string op = currentToken.value;
        advance();
        auto right = parseMultiplicative();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    auto left = parsePrimary();
    
    while (match(TokenType::OPERATOR) && 
           (currentToken.value == "*" || currentToken.value == "/")) {
        std::string op = currentToken.value;
        advance();
        auto right = parsePrimary();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto node = std::make_unique<NumberNode>(currentToken.value);
        advance();
        return node;
    }
    
    if (match(TokenType::STRING)) {
        auto node = std::make_unique<StringNode>(currentToken.value);
        advance();
        return node;
    }
    
    if (match(TokenType::IDENTIFIER)) {
        if (peek().type == TokenType::LPAREN) {
            return parseFunctionCall();
        }
        
        auto node = std::make_unique<IdentifierNode>(currentToken.value);
        advance();
        return node;
    }
    
    if (match(TokenType::LPAREN)) {
        advance();
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Ожидается ')'");
        advance();
        return expr;
    }
    
    if (match(TokenType::KEYWORD) && 
        (currentToken.value == "true" || currentToken.value == "false")) {
        auto node = std::make_unique<IdentifierNode>(currentToken.value);
        advance();
        return node;
    }
    
    std::cerr << "Неожиданный токен: " << currentToken.value << std::endl;
    advance();
    return nullptr;
}

void Parser::printAST(const std::vector<std::unique_ptr<ASTNode>>& ast) {
    std::cout << "\n=== AST (Abstract Syntax Tree) ===\n\n";
    
    for (size_t i = 0; i < ast.size(); ++i) {
        std::cout << i + 1 << ": " << ast[i]->toString() << std::endl;
    }
    
    if (ast.empty()) {
        std::cout << "Программа пуста\n";
    }
}
