// TimerPlay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\include\Ticker.h"
#include <boost\timer\timer.hpp>

std::atomic<bool> hasTicked_;
std::atomic<int> numTicked_;
std::mutex tickedMutex_;
std::condition_variable ticked_;
// tick signal handler
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


boost::signals2::signal<void()> tickPublisher;
/// the ticker thread
std::thread* tickerThread_;
/// the ticker exception
std::exception_ptr exceptionPtr_;
/// the ticker thread control mutex
std::mutex tickingMutex_;
/// the ticker conditional
std::condition_variable tickingCondition_;

class TickWrapper
{
public:

    /// the ticker function
    void Tick(std::chrono::milliseconds period)
    {
        try{
            std::unique_lock<std::mutex> ul(tickingMutex_);
            while(std::cv_status::timeout == tickingCondition_.wait_for(ul, period)){
                tickPublisher();
            }
        }
        catch(...){
            exceptionPtr_ = std::current_exception();
        }

    }
};

std::chrono::milliseconds period(500);

void TestTickerObject()
{
    JuggleLib::Ticker ticker(period);
    JuggleLib::Ticker::Slot slot = std::bind(Ticked);
//    JuggleLib::Ticker::Connection callbackConnection =  ticker.AddTickResponder(slot);
    ticker.Start();
    while(!hasTicked_.load()){
        std::cout << "waiting" << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(250));
    }
    std::cout << "stop Ticker" << std::endl;
    ticker.Stop();
 
}

int _tmain(int argc, _TCHAR* argv[])
{
    hasTicked_ = false;
    numTicked_ = 0;


    std::function<void()> slot = std::bind(Ticked);
    boost::signals2::connection callbackConnection =  tickPublisher.connect(slot);

    {
    boost::timer::auto_cpu_timer timer;
    std::cout << "start Ticker" << std::endl;

    TickWrapper ticker;
    tickerThread_ = new std::thread(std::bind(&TickWrapper::Tick, &ticker, period));
    WaitUntilTicked(1);
    
    std::cout << "stop Ticker" << std::endl;
    tickingCondition_.notify_all();

    }
    tickPublisher.disconnect(callbackConnection);
    std::getchar();
    return 0;
}

