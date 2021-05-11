#include "lexer.hh"
#include "parser.hh"
#include <iostream>
#include <iterator>

int main() {
	auto begin = std::istreambuf_iterator<char>{std::cin};
	auto end = std::istreambuf_iterator<char>{};
	auto ast = parser::Parser{lex::LexemIterator{begin, end}, lex::LexemIterator{end}}.parse();
	std::cout << std::endl << ast->toLatex() << std::endl
		<< ast->deriv()->toLatex() << std::endl
		<< ast->deriv()->optimize()->toLatex() << std::endl;
}
