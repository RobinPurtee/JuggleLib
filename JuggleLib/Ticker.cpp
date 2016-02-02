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


    Ticker::Ticker(millisecond period)
    : period_(period)
    {}
    /// Set the number of milliseconds in the period of the ticker
    /// The period cannot be set while the ticker is running
    void Ticker::setPeriod(millisecond p)
    {
        if(tick_.empty()){
            period_ = p;
        }
        else
        {
            throw new std::exception("The period cannot be set while the ticker is running");
        }
    }

    void Ticker::Start()
    {
        ticker_ = new std::thread<

    }

    void Ticker::Stop()
    {
    }

    Ticker::Connection Ticker::AddTickResponder(Ticker::Slot tickSlot)
    {
    }

    void Ticker::RemoveTickResponder(Ticker::Connection connector)
    {

    }

    void Ticker::RemoveTickResponder(Ticker::Slot tickSlot)
    {
    }

    void Ticker::Tick()
    {
        while(keepTicking && !tick_.empty()){
            std::this_thread::sleep_for(period_);
            tick_();
    }

}
