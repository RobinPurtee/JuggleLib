#include "pch.h"
#include "JuggleLib.h"
#include "Ticker.h"
#include <exception>

namespace JuggleLib
{

    /// default constructor
    Ticker::Ticker()
    : period_(1)
    {}

    /// Initializing constructor
    Ticker::Ticker(millisecond period)
    : period_(period)
    {}
    /// Destructor
    Ticker::~Ticker()
    {
        if(IsRunning()){
            Stop();
        }
    }

    /// Set the number of milliseconds in the period of the ticker
    /// The period of the ticker is the period at the point the ticker is started
    /// If the period is changed once started the new period will not take effect
    /// until the ticker is stopped and re-started.
    void Ticker::setPeriod(millisecond p)
    {
        period_ = p;
    }

    void Ticker::Start()
    {
        if(!IsRunning()){
            keepTicking_ = true;
            auto functor = std::bind(&Ticker::Tick, this, std::placeholders::_1, std::placeholders::_2);
            std::thread* thr = new std::thread(functor, tick_, period_);
            ticker_ = std::make_shared<std::thread>(thr);
        }
        else{
            throw std::runtime_error("Ticker can only be started once");
        }
    }

    bool Ticker::IsRunning()
    {
        return keepTicking_;
    }

    void Ticker::Stop()
    {
        CriticalSection cs(tickingMutex_);
        keepTicking_ = false;
        if(ticker_->joinable()){
            ticker_->join();
        }
    }
    /// Add a tick slot to the ticker
    Ticker::Connection Ticker::AddTickResponder(Ticker::Slot tickSlot)
    {
        return tick_.connect(tickSlot);
    }
    /// remove a connection from the ticker
    void Ticker::RemoveTickResponder(Ticker::Connection connector)
    {
        tick_.disconnect(connector);
    }
    /// remove a slot from the ticker
    void Ticker::RemoveTickResponder(Ticker::Slot tickSlot)
    {
        tick_.disconnect(tickSlot);
    }
    /// Test if the tick thread should keep running in a thread save manor
    bool Ticker::KeepTicking()
    {
        CriticalSection cs(tickingMutex_);
        return keepTicking_;
    }
    /// the ticker thread and method
    void Ticker::Tick(TickPublisher& tick, millisecond period)
    {
        try{
            while(KeepTicking() && !tick_.empty()){
                std::this_thread::sleep_for(period);
                tick();
            }
        }
        catch(...){
            exceptionPtr_ = std::current_exception();
        }
    }

}
