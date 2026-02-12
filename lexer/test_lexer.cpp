#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(){
	// example 1
	std::string code = R"(
		//this comment
		int x = 42;
		if(x > 10){
			x = x + 1;
			printf("Hello world!");
		}
		)";
	Lexer lexer(code);
	std::vector<Token> tokens = lexer.tokensize();
	lexer.printTokens(tokens);

	//example 2 read a file
	/*
	std::ifstream file("test.txt");
	if(file.is_open()){
		std::stringstream buffer;
		buffer << file.rdbuf();

		Lexer fileLexer(buffer.str());
		std::vector<Token> fileTokens = fileLexer.tokensize();
		fileLexer.printTokens(fileTokens);

		file.close();
	}
	*/

	return 0;
}
