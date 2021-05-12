#include "ast.hh"
#include "lexer.hh"
#include <memory>

using namespace AST;

void AST::reset(Node::ChildT& old_ptr, Node *new_ptr) {
	if (old_ptr.get() != new_ptr)
		old_ptr.reset(new_ptr);
}

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
Node* BinOp<lex::Mul>::optimize() {
	auto lhsval = lhs_->getVal();
	auto rhsval = rhs_->getVal();
	if(is<0>(lhsval))
		return lhs_.release()->optimize();
	if(is<0>(rhsval))
		return rhs_.release()->optimize();
	if(is<1>(lhsval))
		return rhs_.release()->optimize();
	if(is<1>(rhsval))
		return lhs_.release()->optimize();
	reset(lhs_, lhs_->optimize());
	reset(rhs_, rhs_->optimize());
	return this;
}

template<>
Node* BinOp<lex::Div>::optimize() {
	if(is<0, 1>(lhs_->getVal()))
		return lhs_.release()->optimize();
	reset(lhs_, lhs_->optimize());
	reset(rhs_, rhs_->optimize());
	return this;
}

template<>
Node* UnOp<lex::Plus>::optimize() {
	return exp_.release()->optimize();
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
