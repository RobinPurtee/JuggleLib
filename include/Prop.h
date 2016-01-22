#pragma once
#include <memory>
#include <forward_list>
#include "Throw.h"

namespace JuggleLib
{
    namespace std = ::std;

    class Hand;

    /// This is a base class for an object being juggled
    class Prop
    {
    public:
         
        Prop(int id);
        virtual ~Prop(void);

        //Properties
        int getId()         {return id_;}

        enum class State:int {
            DROPPED = 0,
            DWELL = 1, 
            FLIGHT = 2, 
            CATCH = 3, 
        };

        Prop::State getState();

        /** 
         * Get the string of the current state name
         */
        const char* getStateName();
        static const char* getStateName(Prop::State state);
    
        /**
         * 
         */
        bool isDropped();
        bool isInFlight();
        bool isInPlay();

        void Toss(Throw& toss);

        void Caught();

        void Collision(JuggleLib::DropReportPtr drop);

        void Pickup(Hand* hand);

        void Tick();
     
        void connectToToss(PropSlot slot);
        void disconnectFromToss(PropSlot slot);

        void connectToDrop(DropReportSlot slot);
        void disconnectFromDrop(DropReportSlot slot);

        void connectToCatch(PropSlot slot);
        void disconnectFromCatch(PropSlot slot);

        void connectToAll(PropSlot tossSlot, DropReportSlot dropSlot, PropSlot propSlot);
        void disconnectFromAll(PropSlot tossSlot, DropReportSlot dropSlot, PropSlot propSlot);

        virtual std::string toString();
        virtual std::wstring toWstring();

        class StateMachine;    
    private:
        void connectHand(Hand* hand);
        void disconnectHand();


        PropPublisher tossed_;
        PropPublisher catch_;
        DropReportPublisher dropped_;
        Throw toss_;
        Hand* hand_;

        int id_;
        std::shared_ptr<Prop::StateMachine> stateMachine_;
    };

    typedef std::shared_ptr<Prop> PropPtr;
    typedef std::list<PropPtr> PropList;
}