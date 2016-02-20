#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Ticker.h"
#include "DebugStringStream.h"
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace JuggleLib;

namespace untitests
{
    TEST_CLASS(TickerTests)
    {
        std::atomic<bool> hasTicked_;
        std::atomic<int> numTicked_;
        std::mutex tickedMutex_;
        std::condition_variable ticked_;

        void Ticked()
        {
            hasTicked_.store(true);
            ++numTicked_;
            ticked_.notify_all();              
        }
        /// wait for Ticked to be called num of times
        void WaitUntilTicked(int num)
        {
            std::unique_lock<std::mutex> ul(tickedMutex_);
                ticked_.wait(ul, [&]{return hasTicked_.load() && num >= numTicked_.load();});
        }

    public:
        
        TEST_METHOD_INITIALIZE(SetupTicking)
        {
            hasTicked_ = false;
            numTicked_ = 0;
        }
        

        TEST_METHOD(SingleTickTest)
        {
            Ticker ticker(std::chrono::milliseconds(5000));

            auto slot = std::bind(&TickerTests::Ticked, this);
            ticker.AddTickResponder(slot);
            DebugOut() << "Start wait" << std::endl;
            ticker.Start();
            WaitUntilTicked(1);
            ticker.Stop();
            DebugOut() << "Ticked" << std::endl;
        }

    };
}