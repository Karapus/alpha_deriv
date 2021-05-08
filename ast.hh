#include <iterator>
#include <string>
#include <memory>

class Node {
public:
	Node *parent = nullptr;
	virtual ~Node() {}
	using ChildT = std::unique_ptr<Node>;
};

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
};

template <typename Op>
class UnOp {
	Node::ChildT exp_;
public:
	UnOp(Node::ChildT exp) : exp_(std::move(exp)) {
		exp_->parent = this;
	}
};

class Num {
	int val_;
public:
	Num(int val) : val_(val) {
	}
};

class Var {
	std::string name_;
public:
	Var(std::string name) : name_(name) {
	}
};
