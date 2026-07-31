// #include <gtest/gtest.h>
// #include "watchdog.hpp"

// TEST(WatchdogTest, DoesNotTimeOutImmediatelyAfterFeed)
// {
//     sentinel::Watchdog wd(std::chrono::milliseconds(50));
//     wd.feed(1);
//     EXPECT_FALSE(wd.timedOut());
// }

// TEST(WatchdogTest, TimesOutAfterNoNewHeartbeat)
// {
//     sentinel::Watchdog wd(std::chrono::milliseconds(50));
//     wd.feed(1);
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     EXPECT_TRUE(wd.timedOut());
// }

// // TODO: write these yourself, following the pattern above:
// // - ResetClearsTimeoutCondition
// // - FeedingSameValueRepeatedlyEventuallyTimesOut
// //   (i.e. calling feed(1), feed(1), feed(1) shouldn't count as "new" heartbeats)