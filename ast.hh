#pragma once
#include <iterator>
#include <string>
#include <memory>
#include <optional>

namespace AST {
class Node {
public:
	using ChildT = std::unique_ptr<Node>;
	virtual ~Node() {}
	virtual ChildT clone() const = 0;
	virtual std::string toLatex() const = 0;
	virtual ChildT deriv() const = 0;
	virtual std::optional<int> getVal() const {
		return std::nullopt;
	}
	template <int... vals>
	static bool is(const std::optional<int> &res) {
		return res && (([res](auto val){ return res == val; })(vals) || ...);
	}
	virtual Node* optimize() {
		return this;
	}
};

void reset(Node::ChildT& old_ptr, Node *new_ptr);

template <typename Op>
char printOp();

template <typename Op>
class UnOp : public Node {
	Node::ChildT exp_;
public:
	UnOp(Node::ChildT exp) : exp_(std::move(exp)) {
	}
	ChildT clone() const override {
		return std::make_unique<UnOp>(exp_->clone());
	}
	std::string toLatex() const override {
		return "(" + std::string{printOp<Op>()} + exp_->toLatex() + ")";
	}
	ChildT deriv() const override {
		return std::make_unique<UnOp>(exp_->deriv());
	}
	Node* optimize() override {
		if (is<0>(exp_->getVal()))
			return exp_.release()->optimize();
		reset(exp_, exp_->optimize());
		return this;
	}
};

template <typename Op>
class BinOp : public Node {
	Node::ChildT lhs_;
	Node::ChildT rhs_;
public:
	BinOp(Node::ChildT lhs, Node::ChildT rhs) :
		lhs_(std::move(lhs)),
		rhs_(std::move(rhs))
	{}
	ChildT clone() const override {
		return std::make_unique<BinOp>(lhs_->clone(), rhs_->clone());
	}
	std::string toLatex() const override {
		return "(" + lhs_->toLatex() + printOp<Op>() + rhs_->toLatex() + ")";
	}
	ChildT deriv() const override {
		return std::make_unique<BinOp>(lhs_->deriv(), rhs_->deriv());
	}
	Node* optimize() override {
		if (Node::is<0>(lhs_->getVal())) {
			reset(rhs_, rhs_.release()->optimize());
			return new UnOp<Op>(std::move(rhs_));
		}
		if (Node::is<0>(rhs_->getVal()))
			return lhs_.release()->optimize();
		reset(lhs_, lhs_->optimize());
		reset(rhs_, rhs_->optimize());
		return this;
	}
};

class Num : public Node {
	int val_;
public:
	Num(int val) : val_(val) {
	}
	ChildT clone() const override {
		return std::make_unique<Num>(val_);
	}
	std::string toLatex() const override {
		return std::to_string(val_);
	}
	ChildT deriv() const override {
		return std::make_unique<Num>(0);
	}
	std::optional<int> getVal() const override {
		return val_;
	}
};

class Var : public Node {
	std::string name_;
public:
	Var(std::string name) : name_(std::move(name)) {
	}
	ChildT clone() const override {
		return std::make_unique<Var>(name_);
	}
	std::string toLatex() const override {
		return name_;
	}
	ChildT deriv() const override {
		return std::make_unique<Var>(name_+"^\\prime");
	}
};

class Fun : public Node {
	std::string name_;
	ChildT exp_;
	ChildT getDerivFun() const;
public:
	Fun(std::string name, ChildT exp) :
		name_(std::move(name)),
		exp_(std::move(exp))
	{}
	ChildT clone() const override {
		return std::make_unique<Fun>(name_, exp_->clone());
	}
	std::string toLatex() const override {
		return name_ + "(" + exp_->toLatex() + ")";
	}
	ChildT deriv() const override;
};

class Tree {
	Node::ChildT root_;
public:
	Tree(Node::ChildT root) : root_(std::move(root))
	{}
	Tree& optimize() {
		reset(root_, root_->optimize());
		return *this;
	}
	Tree deriv() const {
		return Tree(root_->deriv());
	}
	std::string toLatex() const {
		return root_->toLatex();
	}
};
} //namespace AST
