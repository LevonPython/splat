#include <gtest/gtest.h>
#include "sm_splat_manager.h"

class SMSplatManagerTest : public ::testing::Test {
protected:
    SMSplatManager manager;
};

TEST_F(SMSplatManagerTest, BasicFunctionality) {
    std::map<std::string, std::string> user_input = {
        {"receiver_altitude", "100.0"},
        {"view_angle_start", "1.0"},
        {"view_angle_end", "360.0"},
        {"coverage_radius", "25.0"}
    };
    
    EXPECT_NO_THROW(manager.processViewShed(user_input));
} 