#include <iterator>
#include <string>
#include <memory>
#include <iostream>

namespace AST {
class Node {
public:
	Node *parent = nullptr;
	virtual ~Node() {}
	using ChildT = std::unique_ptr<Node>;
	virtual void print() const = 0;
};

template <typename Op>
void printOp();

template <typename Op>
class BinOp : public Node {
	Node::ChildT lhs_;
	Node::ChildT rhs_;
public:
	BinOp(Node::ChildT lhs, Node::ChildT rhs) :
		lhs_(std::move(lhs)),
		rhs_(std::move(rhs))
	{
		lhs_->parent = rhs_->parent = this;
	}
	void print() const override {
		std::cout << '(';
		printOp<Op>();
		std::cout << ' ';
		lhs_->print();
		std::cout << ' ';
		rhs_->print();
		std::cout << ')';
	}
};

template <typename Op>
class UnOp : public Node {
	Node::ChildT exp_;
public:
	UnOp(Node::ChildT exp) : exp_(std::move(exp)) {
		exp_->parent = this;
	}
	void print() const override {
		std::cout << '(';
		printOp<Op>();
		std::cout << ' ';
		exp_->print();
		std::cout << ')';
	}
};

class Num : public Node {
	int val_;
public:
	Num(int val) : val_(val) {
	}
	void print() const override {
		std::cout << val_;
	}
};

class Var : public Node {
	std::string name_;
public:
	Var(std::string name) : name_(name) {
	}
	void print() const override {
		std::cout << name_;
	}
};
} //namespace AST
