#include "pch.h"
#include "JuggleLib.h"
#include "common_state_machine.h"
#include <iostream>


namespace 
{
    using namespace StateMachine;
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(IPropResponder*, responder_)

    BOOST_MSM_EUML_FLAG(isDroppedFlag_);
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);


    /* 
     *  The catch state and support methods
     */
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

    BOOST_MSM_EUML_STATE((catch_entry), CATCH)

    /* 
     *  The dropped state and support methods
     */

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

    BOOST_MSM_EUML_STATE(
        (
            dropped_entry,
            no_action,
            attributes_ << no_attributes_,
            configure_ << isDroppedFlag_
        ), 
        DROPPED
    )
 
    /* 
     *  The flight state and support methods
     */


    BOOST_MSM_EUML_ACTION(tick_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* toss(fsm.get_attribute(Atoss));
            if(nullptr != toss)
            {
                if(0 < toss->siteswap)
                {
                    --toss->siteswap;
                }
                if(0 == toss->siteswap)
                {
                    fsm.process_event(catchEvent);
                }
            }
            
        }
    };

    BOOST_MSM_EUML_STATE(
        (
            no_action,
            no_action,
            attributes_ << no_attributes_,
            configure_ << isInFlightFlag_
        ), 
        FLIGHT
    )



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
            FLIGHT + catchEvent [tick_guard]        == CATCH,
            CATCH + catchEvent                      == DWELL,
            CATCH + tickEvent                       == DROPPED,
            DROPPED + pickupEvent                   == DWELL,
            DWELL + collisionEvent                  == DROPPED,
            FLIGHT + collisionEvent                 == DROPPED,
            CATCH + collisionEvent                  == DROPPED,
            DROPPED + collisionEvent
        )
        , prop_transition_table
    )

    /**
     * Invalid transistion handler
     * in this case could a collisionEvent to force the Dropped state
     */
    BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            fsm.process_event(collisionEvent);
        }
    };

    /**
     * The declaration of the actual state machine type
     */
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE
    ( 
        (
            prop_transition_table, 
            init_ << DROPPED,
            no_action,
            no_action,
            attributes_<< Aid << responder_ << Atoss,
            configure_ << no_configure_,
            invalid_state_transistion
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

struct Prop::PropStateMachine : public Base
{
    PropStateMachine(int id, IPropResponder* responder)
    {
        get_attribute(StateMachine::Aid) = id;
        get_attribute(responder_) = responder;
    }
};

Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(id, this) )
,   id_(id)
{
    stateMachine_->get_attribute(StateMachine::Atoss) = &toss_;
}


Prop::~Prop(void)
{
}

int Prop::getState()
{
    return (*(stateMachine_->current_state()));
}


const TCHAR* Prop::getStateName()
{
    return  stateNames[getState()];
}

bool Prop::isDropped()
{
    return stateMachine_->is_flag_active<isDroppedFlag__helper>();
}


bool Prop::isInFlight()
{
    return stateMachine_->is_flag_active<isInFlightFlag__helper>();
}
        

int Prop::getCurrentSwap()
{
    return toss_.siteswap;
}


bool Prop::isIdValid(int id)
{
    return id == id_;
}

void Prop::Tossed(int id)
{
    if (isIdValid(id) && !tossed.empty())
    {
        tossed(id);
    }
}

void Prop::Catch(int id)
{
    if (isIdValid(id))
    {
        toss_.clear();        
        if( !ready_to_be_caught.empty())
        {
            ready_to_be_caught(id);
        }
    }
    
}

void Prop::Dropped(int id)
{
    if (isIdValid(id) && !dropped.empty())
    {
        dropped(id);
    }
}

/**
 * Start accelorating the prop
 * @remarks 
 * A sucessful toss can only be started if the prop has be caught and in the DWELL state
 */

void Prop::Toss(Throw* toss)
{
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent(toss));
}


void Prop::Catch()
{
    stateMachine_->process_event(StateMachine::catchEvent);
}

void Prop::Collision()
{
    stateMachine_->process_event(StateMachine::collisionEvent);
}

void Prop::Pickup()
{
    stateMachine_->process_event(StateMachine::pickupEvent);
}

void Prop::Tick()
{
    stateMachine_->process_event(StateMachine::tickEvent);
}