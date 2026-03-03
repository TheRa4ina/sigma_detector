#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sigma/rule_parser/selector.h>

using namespace sigma;
using ::testing::Return;
using ::testing::_;

// Mock predicate using Google Mock
class MockPredicate : public IPredicate {
public:
    MOCK_CONST_METHOD2(Match, bool(const std::string& ruleVal, const std::string& matchVal));
};

class SelectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        predMock = std::make_unique<MockPredicate>();
        predMockPtr = predMock.get();
    }

    std::unique_ptr<MockPredicate> predMock;
    MockPredicate* predMockPtr;
};

TEST_F(SelectorTest, Match_SelectorAnyWithAllMatching_True) {
    std::vector<std::string> values = { "a", "b", "c" };
    Selector<MatchesAny> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _))
        .WillRepeatedly(Return(true));

    EXPECT_TRUE(sel.Match("MatchesAll"));
}

TEST_F(SelectorTest, Match_SelectorAnyWithOneMatching_True) {
    std::vector<std::string> values = { "a", "b", "c" };
    Selector<MatchesAny> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_TRUE(sel.Match("MatchesOne"));
}

TEST_F(SelectorTest, Match_SelectorAnyWithNoMatching_False) {
    std::vector<std::string> values = { "a", "b", "c" };
    Selector<MatchesAny> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _))
        .WillRepeatedly(Return(false));

    EXPECT_FALSE(sel.Match("MatchesNone"));
}

TEST_F(SelectorTest, Match_SelectorAllWithAllMatching_True) {
    std::vector<std::string> values = { "x", "y", "z"};
    Selector<MatchesAll> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _))
        .WillRepeatedly(Return(true));

    EXPECT_TRUE(sel.Match("MatchesAll"));
}

TEST_F(SelectorTest, Match_SelectorAllWithOneMatching_False) {
    std::vector<std::string> values = { "x", "y", "z"};
    Selector<MatchesAll> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_FALSE(sel.Match("MatchesOne"));
}

TEST_F(SelectorTest, Match_SelectorAllWithNoMatching_False) {
    std::vector<std::string> values = { "x", "y", "z"};
    Selector<MatchesAll> sel("field", values, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _)).WillRepeatedly(Return(false));

    EXPECT_FALSE(sel.Match("MatchesNone"));
}

TEST_F(SelectorTest, Match_SelectorGroupWithAllMatching_True) {
    std::vector<std::vector<std::string>> groups = {
        {"a", "b"},
        {"c", "d"}
    };
    GroupedSelector sel("field", groups, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _)).WillRepeatedly(Return(true));

    EXPECT_TRUE(sel.Match("MatchesAll"));
}

TEST_F(SelectorTest, Match_SelectorGroupWithOneInGroupMatching_True) {
    std::vector<std::vector<std::string>> groups = {
        {"a", "b"},
        {"c", "d"}
    };
    GroupedSelector sel("field", groups, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match("b", _)).WillRepeatedly(Return(false));
    EXPECT_CALL(*predMockPtr, Match("d", _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*predMockPtr, Match( _ , _)).WillRepeatedly(Return(true));

    EXPECT_TRUE(sel.Match("MatchesOneInEachGroup"));
}

TEST_F(SelectorTest, Match_SelectorGroupWithAllInFirstGroupNoneInOtherMatching_False) {
    std::vector<std::vector<std::string>> groups = {
        {"a", "b"},
        {"c", "d"}
    };
    GroupedSelector sel("field", groups, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match( _ , _)).WillRepeatedly(Return(false));
    EXPECT_CALL(*predMockPtr, Match("a", _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*predMockPtr, Match("b", _)).WillRepeatedly(Return(true));

    EXPECT_FALSE(sel.Match("MatchesOneInFirstGroup"));
}

TEST_F(SelectorTest, Match_SelectorGroupWithNoneInGroupMatching_False) {
    std::vector<std::vector<std::string>> groups = {
        {"a", "b"},
        {"c", "d"}
    };
    GroupedSelector sel("field", groups, std::move(predMock));

    EXPECT_CALL(*predMockPtr, Match(_, _)).WillRepeatedly(Return(false));

    EXPECT_FALSE(sel.Match("MatchesNone"));
}
