// Simple GoogleTest unit tests for DateParser
#include <gtest/gtest.h>
#include "DateParser.h"

TEST(DateParserTest, ParseUSFormat) {
    EXPECT_EQ(DateParser::parse("12/31/2020"), "2020-12-31");
}

TEST(DateParserTest, ParseEUFormat) {
    EXPECT_EQ(DateParser::parse("31/12/2020"), "2020-12-31");
}

TEST(DateParserTest, ParseISOFormat) {
    EXPECT_EQ(DateParser::parse("2020-12-31"), "2020-12-31");
}

TEST(DateParserTest, NormalizeExtractMonth) {
    EXPECT_EQ(DateParser::extractMonth("01-02-2021"), "2021-01");
}

TEST(DateParserTest, InvalidDateThrows) {
    EXPECT_THROW(DateParser::parse("99/99/9999"), std::invalid_argument);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
