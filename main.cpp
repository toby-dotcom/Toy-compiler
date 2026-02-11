#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/ast.hpp"

#include <iostream>
#include <string>

int main() {
    std::string code = "x := 10 + 3.14e-2 * (y ^ 2)";

    lex::Lexer lexer(code);
    parse::Parser parser(lexer);

    try {
        auto tree = parser.parseStatement();
        std::cout << "Parsed AST:\n";
        tree->print();
    } catch(const std::exception& e){
        std::cout << "Parse error: " << e.what() << "\n";
    }

    return 0;
}

