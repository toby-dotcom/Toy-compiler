#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"
#include <vector>
#include <memory>
#include <string>
#include <iostream>


struct ASTNode {
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
};

struct NumberNode : ASTNode {
    std::string value;
    
    NumberNode(const std::string& val) : value(val) {}
    std::string toString() const override {
        return "Number(" + value + ")";
    }
};

struct StringNode : ASTNode {
    std::string value;
    
    StringNode(const std::string& val) : value(val) {}
    std::string toString() const override {
        return "String(\"" + value + "\")";
    }
};

struct IdentifierNode : ASTNode {
    std::string name;
    
    IdentifierNode(const std::string& n) : name(n) {}
    std::string toString() const override {
        return "Identifier(" + name + ")";
    }
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(const std::string& o, 
                 std::unique_ptr<ASTNode> l,
                 std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    
    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

struct VarDeclarationNode : ASTNode {
    std::string type;      // int, float, string, bool
    std::string name;      
    std::unique_ptr<ASTNode> initializer;  // начальное значение
    
    VarDeclarationNode(const std::string& t, const std::string& n,
                      std::unique_ptr<ASTNode> init)
        : type(t), name(n), initializer(std::move(init)) {}
    
    std::string toString() const override {
        return "VarDecl(" + type + " " + name + " = " + 
               (initializer ? initializer->toString() : "?") + ")";
    }
};

struct AssignmentNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    
    AssignmentNode(const std::string& n, std::unique_ptr<ASTNode> v)
        : name(n), value(std::move(v)) {}
    
    std::string toString() const override {
        return "Assignment(" + name + " = " + value->toString() + ")";
    }
};

// If
struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBody;
    std::vector<std::unique_ptr<ASTNode>> elseBody;
    
    IfNode(std::unique_ptr<ASTNode> cond)
        : condition(std::move(cond)) {}
    
    std::string toString() const override {
        std::string result = "If(" + condition->toString() + ")\n  Then: ";
        for (const auto& stmt : thenBody) {
            result += "\n    " + stmt->toString();
        }
        if (!elseBody.empty()) {
            result += "\n  Else:";
            for (const auto& stmt : elseBody) {
                result += "\n    " + stmt->toString();
            }
        }
        return result;
    }
};

// While
struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    WhileNode(std::unique_ptr<ASTNode> cond)
        : condition(std::move(cond)) {}
    
    std::string toString() const override {
        std::string result = "While(" + condition->toString() + ")";
        for (const auto& stmt : body) {
            result += "\n    " + stmt->toString();
        }
        return result;
    }
};

struct FunctionCallNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(const std::string& n) : name(n) {}
    
    std::string toString() const override {
        std::string result = "Call(" + name + ", [";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) result += ", ";
            result += arguments[i]->toString();
        }
        result += "])";
        return result;
    }
};


class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    Token currentToken;

    void advance();           
    Token peek(int offset = 0); 
    bool match(TokenType type); 
    bool match(const std::string& value); 
    void expect(TokenType type, const std::string& errorMessage);
    void expect(const std::string& value, const std::string& errorMessage);
    
    //metods parsing 
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseVarDeclaration();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseFunctionCall();
    
public:
    Parser(const std::vector<Token>& inputTokens);
    
    // main func parser's
    std::vector<std::unique_ptr<ASTNode>> parse();
    

    void printAST(const std::vector<std::unique_ptr<ASTNode>>& ast);
};

#endif
