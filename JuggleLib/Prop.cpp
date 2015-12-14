#include "pch.h"
#include "JuggleLib.h"
#include "common_state_machine.h"
#include <iostream>


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
                responder->Catch(fsm.get_attribute(Aid));
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
                responder->Dropped(fsm.get_attribute(Aid));
            }
        }
    };

    BOOST_MSM_EUML_STATE((catch_entry), CATCH)
    BOOST_MSM_EUML_STATE((dropped_entry), DROPPED)
    BOOST_MSM_EUML_STATE((), FLIGHT)


    BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt ,FSM& fsm,SourceState& ,TargetState& )
        {
            Throw* destinationToss(fsm.get_attribute(Atoss));
            Throw* sourceToss(evt.get_attribute(Atoss));
            if(nullptr != destinationToss && nullptr != sourceToss)
            {
                *destinationToss = *sourceToss;
            }

            IPropResponder* responder(fsm.get_attribute(responder_));
            if(nullptr !=  responder)
            {
                responder->Tossed(fsm.get_attribute(Aid));
            }
        }
    };

    BOOST_MSM_EUML_ACTION(tick_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* toss(fsm.get_attribute(Atoss));
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
            Throw* toss(fsm.get_attribute(Atoss));
            if(nullptr != toss)
            {
                bRet =  0 == toss->siteswap;
            }

            return bRet;
        }
    };



    BOOST_MSM_EUML_TRANSITION_TABLE(
        (
            DWELL + tossEvent / toss_action         == FLIGHT,
            FLIGHT + tickEvent / tick_action,
            FLIGHT + tickEvent [tick_guard]         == CATCH,
            CATCH + catchEvent                      == DWELL,
            CATCH + tickEvent                       == DROPPED,
            DROPPED + pickupEvent                   == DWELL,
            DWELL + collisionEvent                  == DROPPED,
            FLIGHT + collisionEvent                 == DROPPED,
            CATCH + collisionEvent                  == DROPPED
        )
        , prop_transition_table
    )

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE
    ( 
        (
            prop_transition_table, 
            init_ << DROPPED,
            no_action,
            no_action,
            attributes_<< Aid << responder_ << Atoss  
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
        get_attribute(StateMachine::Aid) = id;
        get_attribute(PropStateMachineSpace::responder_) = responder;
    }
};

Prop::Prop(int id_)
:   stateMachine(new Prop::PropStateMachine(id_, this) )
,   id(id_)
{
    stateMachine->get_attribute(StateMachine::Atoss) = &toss;
}


Prop::~Prop(void)
{
}

int Prop::getState()
{
    return (*(stateMachine->current_state()));
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