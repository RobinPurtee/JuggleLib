#include "pch.h"
#include "JuggleLib.h"
#include "Hand.h"
#include "Throw.h"
#include "Prop.h"
#include "common_state_machine.h"


namespace 
{
    using namespace StateMachine;

    BOOST_MSM_EUML_FLAG(vacant_flag_);


    BOOST_MSM_EUML_EVENT(releaseEvent)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, propAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(catchEvent, propAttributes)
    BOOST_MSM_EUML_EVENT(caughtEvent);


    /*
     *  toss state and actions
     */

    BOOST_MSM_EUML_ACTION(toss_entry_action)
    {
        template <class FSM, class EVT, class State>
        void operator()(EVT const& evt, FSM& fsm, State& state)
        {
            DebugOut(_T("HandStateMachine::toss_entry_action"));
        }
    };


    BOOST_MSM_EUML_ACTION(release_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut(_T("HandStateMachine::release_action"));
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        toss_entry_action,
        no_action,
        attributes_ << no_attributes_,
        configure_ << no_configure_
    ), TOSS)

    /**
    * catch state and actions
    */
    BOOST_MSM_EUML_ACTION(catch_entry_action)
    {
        template <class FSM, class EVT, class State>
        void operator()(EVT const& evt, FSM& fsm, State& state )
        {
            DebugOut(_T("HandStateMachine::catch_entry_action"));
            Prop* prop(evt.get_attribute(Aprop));
            if(nullptr != prop){
                state.get_attribute(Aprop) = prop;
            }
        }
    };

    BOOST_MSM_EUML_ACTION(catch_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut(_T("HandStateMachine::catch_exit_action"));
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        catch_entry_action,
        catch_exit_action,
        attributes_ << Aprop,
        configure_ << no_configure_
    ), CATCH)

    /** 
     *  Vacant state
     */ 
     BOOST_MSM_EUML_ACTION(vacant_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut(_T("HandStateMachine::vacant_entry_action"));
        }
    };

     BOOST_MSM_EUML_ACTION(vacant_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut(_T("HandStateMachine::vacant_entry_action"));
        }
    };
    BOOST_MSM_EUML_STATE(
    (
        vacant_entry_action,
        vacant_exit_action,
        attributes_ << no_attributes_,
        configure_ << vacant_flag_
    ), VACANT)


    /** 
     *  Pickup action
     */
    BOOST_MSM_EUML_ACTION(pickup_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut(_T("HandStateMachine::pickup_action"));
        }
    };

    /**
    *  test if the hand is actually vacant
    */
    BOOST_MSM_EUML_ACTION(vacant_guard)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            bool bRet = fsm.get_attribute(Ahand)->isVacant();
            DebugOut() << "HandStateMachine::vacant_guard: returned " << std::boolalpha << bRet;
            return bRet;
        }
    };

    /** 
     *
     */

    BOOST_MSM_EUML_TRANSITION_TABLE
        (
            (
                VACANT + pickupEvent / pickup_action    == DWELL,
                DWELL + tossEvent                       == TOSS,
                TOSS + releaseEvent [vacant_guard]      == VACANT,
                TOSS + releaseEvent [!vacant_guard]     == DWELL,
                VACANT + catchEvent                     == CATCH,
                VACANT [!vacant_guard]                  == DWELL,
                CATCH + caughtEvent                     == DWELL,
                CATCH + collisionEvent                  == VACANT,
                //CATCH + collisionEvent  [!vacant_guard] == DWELL,
                DWELL + pickupEvent / pickup_action             //,
                //DWELL + catchEvent / collision_action           ,
                //DWELL + releaseEvent / collision_action == VACANT 
            )
            , hand_transition_table                     
        )

        /**
        * Invalid transistion handler
        */
        BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            //fsm.process_event(collisionEvent);
        }
    };



    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(
        (
            hand_transition_table,      // The transition table
            init_ << VACANT,            // The initial State
            no_action,                  // The startup action
            no_action,                  // The exit action
            attributes_  << Ahand, // the attributes
            configure_ << no_configure_, // configuration parameters (flags and funcitons)
            invalid_state_transistion    // default action if transition is invalid
        ), hand_state_machine );

    // the type for the state machine

    typedef msm::back::state_machine<hand_state_machine> Base;

    const TCHAR* stateNames[] = {
        TEXT("Vacant"),
        TEXT("Dwell"),
        TEXT("Toss"),
        TEXT("Catch"),
    };

}

struct Hand::HandStateMachine : public Base
{
    HandStateMachine(Hand* hand)
        : Base()
    {
        get_attribute(StateMachine::Ahand) = hand;
    }
};


Hand::Hand(int id)
    : stateMachine_(new HandStateMachine(this))
    , id_(id)
    , toss_(nullptr)
    , propCatching_(nullptr)
{
}


Hand::~Hand(void)
{
}

bool Hand::isVacant()
{
    //return stateMachine_->is_flag_active<vacant_flag__helper>();
    return props_.empty();
}

/**
*
*/

int Hand::getId()
{
    return id_;
}

/**
*
*/

Hand::State Hand::getState()
{
    return (static_cast<Hand::State>(*(stateMachine_->current_state())));
}

/**
*
*/

const TCHAR* Hand::getStateName()
{
    return  stateNames[*(stateMachine_->current_state())];
}


void Hand::Pickup(Prop* prop)
{
    if(nullptr != prop){
        stateMachine_->process_event(pickupEvent(prop));
        props_.push_back(prop);
        prop->Pickup(this);
    }
}


void Hand::Toss(Throw* toss)
{
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent);
    toss_ = toss;
}

void Hand::Release()
{
    if(!props_.empty())
    {
        Prop* prop(*(props_.begin()));
        props_.pop_front();
        prop->Toss(*toss_);
        toss_ = nullptr;
    }
    stateMachine_->process_event(releaseEvent);
}

void Hand::Catch(Prop* prop)
{
    DebugOut(_T("Hand::Catch(%d) in state: %s Hand state: %s") , prop->getId(), prop->getStateName(), getStateName());
    if(nullptr != prop){
        stateMachine_->process_event(catchEvent(prop));
    }

}

void Hand::Caught(Prop* prop)
{
    DebugOut(_T("Hand::caught(%d) in state: %s Hand state: %s"), prop->getId(), prop->getStateName(), getStateName());
    if(nullptr != prop && !prop->isDropped()){
        stateMachine_->process_event(caughtEvent);
        prop->Caught();
        props_.push_front(prop);
    }
}

void Hand::Collision(Prop* prop)
{
    assert(nullptr != prop);
    DebugOut(_T("Hand::Collision(%d) in state: %s Hand state: %s"), prop->getId(), prop->getStateName(), getStateName());
    stateMachine_->process_event(StateMachine::collisionEvent(prop));
}

#if defined(_DEBUG)

#include <sstream>
std::wstring Hand::toString()
{
    std::wstringstream out;
    out << L"Hand id: " << id_ << L" State: " << getStateName() << std::endl;
    for(Prop* p : props_){
        out << L"    Prop id: " << p->getId() << L" State: " << p->getStateName() << std::endl;
    }

    return out.str();
}

#endif // defined(_DEBUG)




