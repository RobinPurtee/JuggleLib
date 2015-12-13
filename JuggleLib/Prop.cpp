#include "pch.h"
#include "JuggleLib.h"
#include "common_state_machine.h"
#include <iostream>


namespace msm = boost::msm;
using namespace boost::msm::front::euml;

namespace PropStateMachineSpace
{
    using namespace StateMachine;
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(IPropResponder*, responder_)


    BOOST_MSM_EUML_ACTION(catch_entry)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            IPropResponder* responder(fsm.get_attribute(responder_));
            if(nullptr !=  responder)
            {
                responder->Catch(fsm.get_attribute(id_));
            }
        }
    };

    BOOST_MSM_EUML_ACTION(dropped_entry)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            IPropResponder* responder(fsm.get_attribute(responder_));
            if(nullptr !=  responder)
            {
                responder->Dropped(fsm.get_attribute(id_));
            }
        }
    };
    BOOST_MSM_EUML_STATE((catch_entry), Catch)
    BOOST_MSM_EUML_STATE((dropped_entry), Dropped)

    BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt ,FSM& fsm,SourceState& ,TargetState& )
        {
            Throw* destinationToss(fsm.get_attribute(toss_));
            Throw* sourceToss(evt.get_attribute(toss_));
            if(nullptr != destinationToss && nullptr != sourceToss)
            {
                *destinationToss = *sourceToss;
            }

            IPropResponder* responder(fsm.get_attribute(responder_));
            if(nullptr !=  responder)
            {
                responder->Tossed(fsm.get_attribute(id_));
            }
        }
    };

    BOOST_MSM_EUML_ACTION(tick_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* toss(fsm.get_attribute(toss_));
            if(nullptr != toss && 0 < toss->siteswap)
            {
                --toss->siteswap;
            }
        }
    };


    BOOST_MSM_EUML_ACTION(tick_guard)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            bool bRet(false);
            Throw* toss(fsm.get_attribute(toss_));
            if(nullptr != toss)
            {
                bRet =  0 == toss->siteswap;
            }

            return bRet;
        }
    };



    BOOST_MSM_EUML_TRANSITION_TABLE(
        (
            Dwell + tossEvent / toss_action         == Flight,
            Flight + tickEvent / tick_action,
            Flight + tickEvent [tick_guard]         == Catch,
            Catch + catchEvent                      == Dwell,
            Catch + tickEvent                       == Dropped,
            Dropped + pickupEvent                   == Dwell,
            Dwell + collisionEvent                  == Dropped,
            Flight + collisionEvent                 == Dropped,
            Catch + collisionEvent                  == Dropped
        )
        , prop_transition_table
    )

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE
    ( 
        (
            prop_transition_table, 
            init_ << Dropped,
            no_action,
            no_action,
            attributes_<< id_ << responder_ << toss_  
        ), 
        prop_state_machine
    )

    // the type for the state machine

    typedef msm::back::state_machine<prop_state_machine> Base;
    const TCHAR* stateNames[] = {
        TEXT("Dwell"),
        TEXT("Flight"),
        TEXT("Catch"),
        TEXT("Dropped"),
    };

}

struct Prop::PropStateMachine : public PropStateMachineSpace::Base
{
    PropStateMachine(int id, IPropResponder* responder)
    {
        get_attribute(StateMachine::id_) = id;
        get_attribute(PropStateMachineSpace::responder_) = responder;
    }
};

Prop::Prop(int id_)
:   stateMachine(new Prop::PropStateMachine(id_, this) )
,   id(id_)
{
    stateMachine->get_attribute(StateMachine::toss_) = &toss;
}


Prop::~Prop(void)
{
}

Prop::State Prop::getState()
{
    return static_cast<Prop::State>(*(stateMachine->current_state()));
}


const TCHAR* Prop::getStateName()
{
    return  PropStateMachineSpace::stateNames[getState()];
}


int Prop::getCurrentSwap()
{
    return toss.siteswap;
}


bool Prop::isIdValid(int id_)
{
    return id_ == id;
}

void Prop::Tossed(int id_)
{
    if (isIdValid(id_) && !tossed.empty())
    {
        tossed(id_);
    }
}

void Prop::Catch(int id_)
{
    if (isIdValid(id_))
    {
        toss.clear();        
        if( !ready_to_be_caught.empty())
        {
            ready_to_be_caught(id_);
        }
    }
    
}

void Prop::Dropped(int id_)
{
    if (isIdValid(id_) && !dropped.empty())
    {
        dropped(id_);
    }
}

/**
 * Start accelorating the prop
 * @remarks 
 * A sucessful toss can only be started if the prop has be caught and in the DWELL state
 */

void Prop::Toss(Throw* toss_)
{
    assert(nullptr != toss_);
    stateMachine->process_event(StateMachine::tossEvent(toss_));
}


void Prop::Catch()
{
    stateMachine->process_event(StateMachine::catchEvent);
}

void Prop::Collision()
{
    stateMachine->process_event(StateMachine::collisionEvent);
}

void Prop::Pickup()
{
    stateMachine->process_event(StateMachine::pickupEvent);
}

void Prop::Tick()
{
    stateMachine->process_event(StateMachine::tickEvent);
}