// TimerPlay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <boost/signals2.hpp>

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


int _tmain(int argc, _TCHAR* argv[])
{
    hasTicked_ = false;
    numTicked_ = 0;

    std::chrono::milliseconds period_(1000);

    //JuggleLib::Ticker::TickPublisher::slot_type& slot = std::bind(&TickerTests::Ticked, this);
    std::function<void()> slot = std::bind(Ticked);
    boost::signals2::connection callbackConnection =  tickPublisher.connect(slot);

    std::cout << "start Ticker" << std::endl;
    tickerThread_ = new std::thread(Tick, period_);
    WaitUntilTicked(1);
    std::cout << "stop Ticker" << std::endl;
    tickPublisher.disconnect(callbackConnection);
    return 0;
}

