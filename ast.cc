#include "ast.hh"
#include "lexer.hh"
#include <memory>

using namespace AST;

template<>
char AST::printOp<lex::Plus>() {
	return '+';
}
template<>
char AST::printOp<lex::Minus>() {
	return '-';
}
template<>
char AST::printOp<lex::Mul>() {
	return '*';
}
template<>
char AST::printOp<lex::Div>() {
	return '/';
}

template<>
std::string BinOp<lex::Div>::toLatex() const {
	return "\\frac{" + lhs_->toLatex() + "}{" + rhs_->toLatex() + "}";
}

template<>
Node::ChildT BinOp<lex::Mul>::deriv() const {
	auto prod1 = std::make_unique<BinOp<lex::Mul>>(lhs_->deriv(), rhs_->clone());
	auto prod2 = std::make_unique<BinOp<lex::Mul>>(lhs_->clone(), rhs_->deriv());
	return std::make_unique<BinOp<lex::Plus>>(std::move(prod1), std::move(prod2));
}

template<>
Node::ChildT BinOp<lex::Div>::deriv() const {
	auto num1 = std::make_unique<BinOp<lex::Mul>>(lhs_->deriv(), rhs_->clone());
	auto num2 = std::make_unique<BinOp<lex::Mul>>(lhs_->clone(), rhs_->deriv());
	auto numer = std::make_unique<BinOp<lex::Minus>>(
			std::move(num1), std::move(num2));
	auto denom = std::make_unique<BinOp<lex::Mul>>(rhs_->clone(), rhs_->clone());
	return std::make_unique<BinOp<lex::Div>>(std::move(numer), std::move(denom));
}

template<>
Node::ChildT BinOp<lex::Mul>::optimize() const {
	auto lhsval = lhs_->getVal();
	auto rhsval = rhs_->getVal();
	if(is<0>(lhsval))
		return lhs_->optimize();
	if(is<0>(rhsval))
		return rhs_->optimize();
	if(is<1>(lhsval))
		return rhs_->optimize();
	if(is<1>(rhsval))
		return lhs_->optimize();
	return std::make_unique<BinOp>(lhs_->optimize(), rhs_->optimize());
}

template<>
Node::ChildT BinOp<lex::Div>::optimize() const {
	if(is<0, 1>(lhs_->getVal()))
		return lhs_->optimize();
	return std::make_unique<BinOp>(lhs_->optimize(), rhs_->optimize());
}

template<>
Node::ChildT UnOp<lex::Plus>::optimize() const {
	return exp_->optimize();
}

Node::ChildT Fun::deriv() const {
		return std::make_unique<BinOp<lex::Mul>>(
				getDerivFun(),
				exp_->deriv());
}

Node::ChildT Fun::getDerivFun() const {
	if (name_ == "sin")
		return std::make_unique<Fun>("cos", exp_->clone());
	if (name_ == "cos")
		return std::make_unique<UnOp<lex::Minus>>(
				std::make_unique<Fun>("sin", exp_->clone()));
	return std::make_unique<Fun>(name_ + "^\\prime", exp_->clone());
}
