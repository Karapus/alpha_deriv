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
	std::vector v {lex::LexemIterator{b, e}, lex::LexemIterator{e}};
	EXPECT_EQ(std::get<lex::Num>(v[4]).val, 42);
	EXPECT_EQ(std::get<lex::Num>(v[6]).val, 3);
}
}
