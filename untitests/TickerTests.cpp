#include "stdafx.h"
#include "CppUnitTest.h"
#include "Ticker.h"
#include "DebugStringStream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
        bool WaitUntilTicked(int num)
        {
            std::unique_lock<std::mutex> ul(tickedMutex_);
            ticked_.wait(ul, [&]{return hasTicked_.load() && num >= numTicked_.load();});
            return true;
        }

    public:
        
        TEST_METHOD_INITIALIZE(SetupTicking)
        {
            hasTicked_ = false;
            numTicked_ = 0;
        }
        

        TEST_METHOD(SingleTickTest)
        {
            JuggleLib::Ticker ticker(std::chrono::milliseconds(1000));

            //JuggleLib::Ticker::TickPublisher::slot_type& slot = std::bind(&TickerTests::Ticked, this);
            std::function<void()> slot = std::bind(&TickerTests::Ticked, this);
            ticker.AddTickResponder(slot);

             DebugOut() << "start Ticker" << std::endl;
             ticker.Start();
             WaitUntilTicked(1);
             DebugOut() << "stop Ticker" << std::endl;


        }

    };
}