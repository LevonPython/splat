#include <gtest/gtest.h>
#include "splat.h"
#include "splat_config.h"
#include <cstring>
#include <string>

class SplatTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test HD_MODE configuration
TEST_F(SplatTest, ConfigurationTest) {
    #if HD_MODE
        EXPECT_EQ(HD_MODE, 1);
    #else
        EXPECT_EQ(HD_MODE, 0);
    #endif
    
    // Test MAXPAGES is within valid range
    EXPECT_GE(MAXPAGES, 1);
    EXPECT_LE(MAXPAGES, 64);
}

// Add more tests as needed

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
