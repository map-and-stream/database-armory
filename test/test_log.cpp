#include <gtest/gtest.h>
#include "factory.h"
#include <sstream>
#include <iostream>

TEST(SampleTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(LogTest, InfoOutput) {
    ILogger* l = LoggerFactory::createLogger(LoggerType::Console);
    testing::internal::CaptureStdout();
    l->info("Hello");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("[INFO] Hello"), std::string::npos);
}

// TEST(LogTest, WarnOutput) {
//     testing::internal::CaptureStdout();
//     log::warn("Warning");
//     std::string output = testing::internal::GetCapturedStdout();
//     EXPECT_NE(output.find("[WARN] Warning"), std::string::npos);
// }

// TEST(LogTest, ErrorOutput) {
//     testing::internal::CaptureStderr();
//     log::error("Error");
//     std::string output = testing::internal::GetCapturedStderr();
//     EXPECT_NE(output.find("[ERROR] Error"), std::string::npos);
// }
