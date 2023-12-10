#include <CppUTest/TestHarness.h>
#include <CppUTest/UtestMacros.h>

TEST_GROUP(UnitTest){};

TEST(UnitTest, Queue) {
    CHECK_TRUE(queue_bids());
    CHECK_TRUE(queue_ask());
}

Test(UnitTest, Inserts) {
    CHECK_TRUE(insert_marketable_bid());
    CHECK_TRUE(insert_marketable_ask());
}

