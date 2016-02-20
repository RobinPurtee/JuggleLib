#include "pch.h"
#include "JuggleLib.h"
#include "Ticker.h"
#include <exception>

namespace JuggleLib
{

    /// default constructor
    Ticker::Ticker()
    : period_(1)
    , ticker_(nullptr)
    , keepTicking_(false)
    {}

    /// Initializing constructor
    Ticker::Ticker(std::chrono::milliseconds period)
    : period_(period)
    , ticker_(nullptr)
    , keepTicking_(false)
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
    void Ticker::setPeriod(std::chrono::milliseconds p)
    {
        period_ = p;
    }

    void Ticker::Start()
    {
        if(!IsRunning()){
            keepTicking_ = true;
            //auto functor = std::bind<void(std::chrono::milliseconds)>(&Ticker::Tick, this, std::placeholders::_1);
            //std::thread* thr = new std::thread(&Ticker::Tick, this, period_);
            ticker_ = new std::thread(&Ticker::Tick, this, period_);
        }
        else{
            throw std::runtime_error("Ticker can only be started once");
        }
    }

    bool Ticker::IsRunning()
    {
        return nullptr != ticker_ && keepTicking_;
    }

    void Ticker::Stop()
    {
        CriticalSection cs(tickingMutex_);
        keepTicking_ = false;
        if(nullptr != ticker_){
            if(ticker_->joinable()){
                ticker_->join();
            }
            delete ticker_;
            ticker_ = nullptr;
        }
    }
    /// Add a tick slot to the ticker
    Ticker::Connection Ticker::AddTickResponder(const Ticker::Slot& tickSlot)
    {
        return tick_.connect(tickSlot);
    }
    /// remove a connection from the ticker
    void Ticker::RemoveTickResponder(Ticker::Connection& connector)
    {
        tick_.disconnect(connector);
    }
    /// remove a slot from the ticker
    //void Ticker::RemoveTickResponder(Ticker::TickPublisher::slot_type& tickSlot)
    //{
    //    tick_.disconnect(tickSlot);
    //}
    /// Test if the tick thread should keep running in a thread save manor
    bool Ticker::KeepTicking()
    {
        CriticalSection cs(tickingMutex_);
        return keepTicking_ && !tick_.empty();
    }
    /// the ticker thread and method
    void Ticker::Tick(std::chrono::milliseconds period)
    {
        try{
            std::unique_lock<std::mutex> ul(tickingMutex_);
            while(KeepTicking() && std::cv_status::timeout == tickingCondition_.wait_for(ul, period)){
                tick_();
            }
        }
        catch(...){
            exceptionPtr_ = std::current_exception();
        }

    }

}
