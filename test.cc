#include <gtest/gtest.h>
#include "lexer.hh"
#include <string_view>
#include <vector>
#include <algorithm>

namespace {
TEST(lex, lexems) {
	std::string_view s = "name +bigname /42 * 3";
	auto b = s.begin();
	auto e = s.end();
	auto lb = lex::LexemIterator{b, e};
	auto le = lex::LexemIterator{e};
	std::vector v(lb, le);
	EXPECT_EQ(std::get<lex::Num>(v[4]).val, 42);
	EXPECT_EQ(std::get<lex::Num>(v[6]).val, 3);
	EXPECT_EQ(std::get<lex::Var>(v[0]).name, "name");
	EXPECT_EQ(std::get<lex::Var>(v[2]).name, "bigname");
}
}
