#pragma once
#include <functional>
#include <chrono>
#include <boost/signals2.hpp>

namespace JuggleLib
{
    namespace std = ::std;

    typedef std::chrono::duration<int, std::milli> millisecond;
    /**
     * This class emittes a signal every duration of milliseconds while running.
     * The signal is emitted from on its own thread. The Slot handler must handle
     * any threading issues.
     */
    class Ticker
    {
        typedef boost::signals2::signal<void()> TickPublisher;
    public:

        typedef std::function<void()> Slot;
        typedef boost::signals2::connection Connection;

        Ticker();
        Ticker(millisecond period);

        millisecond getPeriod()             {return period_;}
        void setPeriod(millisecond p);

        void Start();
        void Stop();

        Connection AddTickResponder(Slot tickSlot);
        void RemoveTickResponder(Connection connector);
        void RemoveTickResponder(Slot tickSlot);

    private:
        void Tick();


        millisecond period_;
        TickPublisher tick_;
        std::shared_ptr<std::thread> ticker_;
    };
}