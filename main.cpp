#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    std::string code = R"(
        // Объявление переменных
        int x = 42;
        int y = 10;
        
        // Присваивание
        x = x + 5;
        
        // Условие
        if (x > y) {
            x = x - y;
        } else {
            y = y + 1;
        }
        
        // Цикл
        int i = 0;
        while (i < 5) {
            i = i + 1;
        }
        
        // Вызов функции
        print("Hello, World!");
    )";
    
    try {
        std::cout << "--- ЛЕКСИЧЕСКИЙ АНАЛИЗ ---" << std::endl;
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokensize();
        lexer.printTokens(tokens);
        
        std::cout << "\n--- СИНТАКСИЧЕСКИЙ АНАЛИЗ ---" << std::endl;
        Parser parser(tokens);
        auto ast = parser.parse();
        
        
        parser.printAST(ast);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

