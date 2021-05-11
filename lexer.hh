#pragma once
#include <cctype>
#include <iterator>
#include <string>
#include <type_traits>
#include <variant>
#include <iterator>

namespace lex {

struct Plus {};
struct Minus {};
struct Mul {};
struct Div {};
struct Lpar {};
struct Rpar {};

struct Num {
	int val;
	template <typename It>
	Num(It& it, It end) {
		std::string res;
		do
			res.push_back(*it++);
		while (it != end && std::isdigit(*it));
		val = std::stoi(res);
	}
};

struct Var {
	std::string name;
	template <typename It>
	Var(It& it, It end) {
		do
			name.push_back(*it++);
		while (it != end && std::isalnum(*it));
	}
};

using LexT = std::variant<std::monostate, Plus, Minus, Mul, Div, Num, Var, Lpar, Rpar>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename It>
class LexemIterator {
	It it_;
	const It end_;
	bool past_;
	LexT lex_;
	void getLexem() {
		char c = *it_;
		if (std::isalpha(c))
			lex_ = Var{it_, end_};
		else if (std::isdigit(c))
			lex_ = Num{it_, end_};
		else {
			switch (c) {
				case '+':
					lex_ = Plus{};
					break;
				case '-':
					lex_ = Minus{};
					break;
				case '*':
					lex_ = Mul{};
					break;
				case '/':
					lex_ = Div{};
					break;
				case '(':
					lex_ = Lpar{};
					break;
				case ')':
					lex_ = Rpar{};
					break;
				default:
					lex_ = LexT{};
					return;
			}
			++it_;
		}
	}
	void skipWS() {
		while (it_ != end_ && std::isspace(*it_))
			it_++;
	}
public:
	using iterator_category = std::input_iterator_tag;
	using value_type = LexT;
	using difference_type = typename std::iterator_traits<It>::difference_type;
	using pointer = void;
	using reference = value_type;
	LexemIterator(It end) : it_(end), end_(end), past_(true)
	{}
	LexemIterator(It it, It end) : it_(it), end_(end), past_(false)
	{
		operator ++();
	}
	value_type operator *() const {
		return lex_;
	}
	auto& operator ++() {
		skipWS();
		if (it_ != end_)
			getLexem();
		else
			past_ = true;
		return *this;
	}
	auto operator ++(int) {
		auto tmp = *this;
		operator ++();
		return tmp;
	}
	auto equal(const LexemIterator &rhs) const {
		return it_ == rhs.it_ && past_ == rhs.past_;
	}
};

template <typename It>
auto operator == (const LexemIterator<It> &lhs, const LexemIterator<It> rhs) {
	return lhs.equal(rhs);
}

template <typename It>
auto operator != (const LexemIterator<It> &lhs, const LexemIterator<It> rhs) {
	return !(lhs == rhs);
}

}//namespace lex
