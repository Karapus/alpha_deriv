#pragma once
#include "ast.hh"
#include "lexer.hh"
#include <memory>
#include <variant>

namespace parser {

using NodePtr = std::unique_ptr<AST::Node>;

template <template <typename> typename Node,
	 typename T, typename... Ts,
	 typename V, typename... Args>
NodePtr makeOp(V&& variant, NodePtr lhs, Args&&... args) {
	if (std::holds_alternative<T>(std::forward<V>(variant)))
		return static_cast<NodePtr>(
				std::make_unique<Node<T>>(
					std::move(lhs), args()...));
	if constexpr (sizeof...(Ts) > 0)
		return makeOp<Node, Ts...>(std::forward<V>(variant),
				std::move(lhs),
				std::forward<Args>(args)...);
	return lhs;
}

template <typename It>
class Parser {
private:
	It it_;
	const It end_;
	lex::LexT getLexem() const {
		if (it_ != end_)
			return *it_;
		return lex::LexT{};
	}
	void advance() {
		if (it_ != end_)
			++it_;
	}
	NodePtr getExpr() {
		auto lhs = getTerm();
		auto GetNext = [this](){
			advance();
			return getExpr();
		};
		return makeOp<AST::BinOp, lex::Plus, lex::Minus>(
				getLexem(), std::move(lhs), std::move(GetNext));
	}
	NodePtr getTerm() {
		auto lhs = getFact();
		auto GetNext = [this](){
			advance();
			return getTerm();
		};
		return makeOp<AST::BinOp, lex::Mul, lex::Div>(
				getLexem(), std::move(lhs), std::move(GetNext));
	}
	NodePtr getFact() {
		auto lex = getLexem();
		advance();
		if (std::holds_alternative<lex::Num>(lex)) {
			return static_cast<NodePtr>(std::make_unique<AST::Num>(std::get<lex::Num>(lex).val));
		}
		if (std::holds_alternative<lex::Var>(lex)) {
			return static_cast<NodePtr>(std::make_unique<AST::Var>(std::get<lex::Var>(lex).name));
		}
		if (std::holds_alternative<lex::Lpar>(lex)) {
			auto res = getExpr();
			if (std::holds_alternative<lex::Rpar>(getLexem())) {
				advance();
				return res;
			}
		}
		return nullptr;
	}
public:
	Parser(It it, It end) : it_(it), end_(end) {
	}
	NodePtr parse() {
		return getExpr();
	}
};

}// namespace parser
