#include <gtest/gtest.h>
#include <sigma/rule_parser/utilities.h>

using namespace utils;

class WildcardMatchTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Tests for IsWildcardMatch (supports '*' and '?')
TEST_F(WildcardMatchTest, IsWildcardMatch_ExactMatch) {
    EXPECT_TRUE(IsWildcardMatch("hello", "hello"));
    EXPECT_FALSE(IsWildcardMatch("hello", "hell"));
    EXPECT_FALSE(IsWildcardMatch("hell", "hello"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_QuestionMark) {
    // '?' matches any single mandatory character
    EXPECT_TRUE(IsWildcardMatch("h?llo", "hello"));
    EXPECT_TRUE(IsWildcardMatch("h?llo", "hxllo"));
    EXPECT_FALSE(IsWildcardMatch("h?llo", "helloo"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_SingleStar) {
    // '*' matches unbounded length wildcard.
    EXPECT_TRUE(IsWildcardMatch("he*", "hello"));
    EXPECT_TRUE(IsWildcardMatch("he*o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("he*", "he"));
    EXPECT_TRUE(IsWildcardMatch("*lo", "hello"));
    EXPECT_TRUE(IsWildcardMatch("h*o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*", ""));
    EXPECT_FALSE(IsWildcardMatch("hello*", "hell"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_MultipleStars) {
    EXPECT_TRUE(IsWildcardMatch("h*e*l*o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*e*o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("h**o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("**", "hello"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_CombinedWildcards) {
    // Combined '?' and '*' usage
    EXPECT_TRUE(IsWildcardMatch("h*?o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("h*?", "hello"));
    EXPECT_TRUE(IsWildcardMatch("?e*o", "hello"));
    EXPECT_TRUE(IsWildcardMatch("h?l*", "hello"));

    EXPECT_TRUE(IsWildcardMatch("*.txt", "file.txt"));
    EXPECT_TRUE(IsWildcardMatch("file*.txt", "file123.txt"));
    EXPECT_TRUE(IsWildcardMatch("file?.txt", "file1.txt"));
    EXPECT_FALSE(IsWildcardMatch("file?.txt", "file123.txt"));
    EXPECT_TRUE(IsWildcardMatch("f*e*t", "file_content_test"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_EmptyStrings) {
    EXPECT_TRUE(IsWildcardMatch("", ""));
    EXPECT_FALSE(IsWildcardMatch("", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*", ""));
    EXPECT_FALSE(IsWildcardMatch("?", ""));
    EXPECT_FALSE(IsWildcardMatch("hello", ""));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_EdgeCases) {
    EXPECT_TRUE(IsWildcardMatch("****", "hello"));
    EXPECT_TRUE(IsWildcardMatch("****", ""));
    EXPECT_TRUE(IsWildcardMatch("*?*", "hello"));
    EXPECT_TRUE(IsWildcardMatch("?*", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*?", "hello"));
    EXPECT_TRUE(IsWildcardMatch("*aa?", "aaaaa"));
    EXPECT_FALSE(IsWildcardMatch("EscapeSymbol\\", "EscapeSymbol\\"));
}

TEST_F(WildcardMatchTest, IsWildcardMatch_EscapeSymbols) {
    EXPECT_TRUE(IsWildcardMatch("StarSymbol\\*", "StarSymbol*"));
    EXPECT_FALSE(IsWildcardMatch("StarSymbol\\*", "StarSymbol"));
    EXPECT_FALSE(IsWildcardMatch("StarSymbol\\*", "StarSymbolX"));
    EXPECT_FALSE(IsWildcardMatch("StarSymbol\\*", "StarSymbolTest"));
    EXPECT_TRUE(IsWildcardMatch("QuestionMarkSymbol\\?", "QuestionMarkSymbol?"));
    EXPECT_FALSE(IsWildcardMatch("QuestionMarkSymbol\\?", "QuestionMarkSymbolX"));
    EXPECT_FALSE(IsWildcardMatch("QuestionMarkSymbol\\?", "QuestionMarkSymbolTest"));
    EXPECT_TRUE(IsWildcardMatch("EscapeSymbol\\\\", "EscapeSymbol\\"));
}

// Tests for ConditionPatternMatch (supports '*' only)
TEST_F(WildcardMatchTest, ConditionPatternMatch_ExactMatch) {
    EXPECT_TRUE(ConditionPatternMatch("hello", "hello"));
    EXPECT_FALSE(ConditionPatternMatch("hello", "hell"));
    EXPECT_FALSE(ConditionPatternMatch("hell", "hello"));
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_SingleStar) {
    // '*' matches zero or more characters
    EXPECT_TRUE(ConditionPatternMatch("he*", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("he*o", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("he*", "he"));
    EXPECT_TRUE(ConditionPatternMatch("*lo", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("h*o", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("*", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("*", ""));
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_MultipleStars) {
    EXPECT_TRUE(ConditionPatternMatch("h*e*l*o", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("*e*o", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("h**o", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("**", "hello"));
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_QuestionMarkLiteral) {
    // '?' should be treated as literal character in ConditionPatternMatch
    EXPECT_FALSE(ConditionPatternMatch("h?llo", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("h?llo", "h?llo"));
    EXPECT_FALSE(ConditionPatternMatch("te?t", "test"));
    EXPECT_FALSE(ConditionPatternMatch("te?t", "text"));
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_EmptyStrings) {
    EXPECT_TRUE(ConditionPatternMatch("", ""));
    EXPECT_FALSE(ConditionPatternMatch("", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("*", ""));
    EXPECT_FALSE(ConditionPatternMatch("hello", ""));
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_EdgeCases) {
    EXPECT_TRUE(ConditionPatternMatch("****", "hello"));
    EXPECT_TRUE(ConditionPatternMatch("****", ""));
    EXPECT_FALSE(ConditionPatternMatch("*?*", "hello"));  // '?' is literal
    EXPECT_TRUE(ConditionPatternMatch("*?*", "hel?lo")); // Matches literal '?'
}

TEST_F(WildcardMatchTest, ConditionPatternMatch_ComplexPatterns) {
    EXPECT_TRUE(ConditionPatternMatch("*.txt", "file.txt"));
    EXPECT_TRUE(ConditionPatternMatch("file*.txt", "file123.txt"));
    EXPECT_FALSE(ConditionPatternMatch("file?.txt", "file1.txt"));  // '?' is literal
    EXPECT_TRUE(ConditionPatternMatch("file?.txt", "file?.txt"));   // Exact match
    EXPECT_TRUE(ConditionPatternMatch("f*e*t", "file_content_test"));
}