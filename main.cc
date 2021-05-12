#include "ast.hh"
#include "lexer.hh"
#include "parser.hh"
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>

std::ostream& operator <<(std::ostream& os, const AST::Tree& ast) {
	return os << "$$" << ast.toLatex() << "$$";
}

int main() {
	auto begin = std::istreambuf_iterator<char>{std::cin};
	auto end = std::istreambuf_iterator<char>{};
	auto ast = AST::Tree{parser::Parser{lex::LexemIterator{begin, end}, lex::LexemIterator{end}}.parse()};
	std::cout << "\\documentclass[a4paper,12pt]{article}" << std::endl
		<< "\\pagestyle{empty}" << std::endl
		<< "\\begin{document}" << std::endl
		<< std::endl << ast << std::endl
		<< ast.deriv() << std::endl
		<< ast.deriv().optimize() << std::endl
		<< "\\end{document}" << std::endl;
}
