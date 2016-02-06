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

        typedef std::function<void(TickPublisher&, millisecond)> Slot;
        typedef boost::signals2::connection Connection;

        Ticker();
        Ticker(millisecond period);

        virtual ~Ticker();

        millisecond getPeriod()             {return period_;}
        void setPeriod(millisecond p);

        void Start();
        bool IsRunning();
        void Stop();

        Connection AddTickResponder(Slot tickSlot);
        void RemoveTickResponder(Connection connector);
        void RemoveTickResponder(Slot tickSlot);

    protected:
        bool KeepTicking();

    private:
        
        bool keepTicking_;
        std::mutex tickingMutex_;
        std::condition_variable tickingCondition_;

        void Tick(TickPublisher& tick, millisecond period);


        millisecond period_;
        TickPublisher tick_;
        std::shared_ptr<std::thread> ticker_;
        std::exception_ptr exceptionPtr_;
        // noncopy 
        Ticker(const Ticker&);
        Ticker& operator=(const Ticker&);


    };
}
