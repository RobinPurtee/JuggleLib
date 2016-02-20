#pragma once
#include <functional>
#include <chrono>
#include <boost/signals2.hpp>
#include <thread>

namespace JuggleLib
{
    namespace std = ::std;

    /**
     * This class emittes a signal every duration of milliseconds while running.
     * The signal is emitted from on its own thread. The Slot handler must handle
     * any threading issues.
     */
    class Ticker
    {
    public:
        typedef boost::signals2::signal<void()> TickPublisher;
        typedef boost::signals2::connection Connection;

        Ticker();
        Ticker(std::chrono::milliseconds period);

        virtual ~Ticker();

        std::chrono::milliseconds getPeriod()             {return period_;}
        void setPeriod(std::chrono::milliseconds p);

        void Start();
        bool IsRunning();
        void Stop();

        Connection AddTickResponder(TickPublisher::slot_type& tickSlot);
        void RemoveTickResponder(Connection& connector);
        //void RemoveTickResponder(TickPublisher::slot_type& tickSlot);

    protected:
        bool KeepTicking();

    private:
        
        bool keepTicking_;
        std::mutex tickingMutex_;
        std::condition_variable tickingCondition_;

        void Tick(std::chrono::milliseconds period);


        std::chrono::milliseconds period_;
        TickPublisher tick_;
        std::thread* ticker_;
        std::exception_ptr exceptionPtr_;
        // noncopy 
        Ticker(const Ticker&);
        Ticker& operator=(const Ticker&);


    };
}
