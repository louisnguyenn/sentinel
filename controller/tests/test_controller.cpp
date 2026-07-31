#include <gtest/gtest.h>

#include <thread>

#include "watchdog.hpp"

TEST(WatchdogTest, DoesNotTimeOutImmediatelyAfterFeed)
{
    sentinel::Watchdog wd(std::chrono::milliseconds(50));
    wd.feed(1);
    EXPECT_FALSE(wd.timedOut());
}

TEST(WatchdogTest, TimesOutAfterNoNewHeartbeat)
{
    sentinel::Watchdog wd(std::chrono::milliseconds(50));
    wd.feed(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(wd.timedOut());
}

// TODO:
// - ResetClearsTimeoutCondition
// - FeedingSameValueRepeatedlyEventuallyTimesOut
//   (i.e. calling feed(1), feed(1), feed(1) shouldn't count as "new" heartbeats)