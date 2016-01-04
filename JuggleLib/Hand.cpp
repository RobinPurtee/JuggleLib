#include "pch.h"
#include "JuggleLib.h"
#include "Hand.h"
#include "Throw.h"
#include "Prop.h"
#include "common_state_machine.h"
#include <sstream>


namespace 
{
    using namespace StateMachine;

    BOOST_MSM_EUML_FLAG(vacant_flag_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(std::deque<Prop*>, props_);


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
            DebugOut() << "HandStateMachine::toss_entry_action";
            Throw* evtToss(evt.get_attribute(Atoss));
            if(nullptr != evtToss){
                state.get_attribute(Atoss) = evtToss;
            }
        }
    };


    BOOST_MSM_EUML_ACTION(release_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "HandStateMachine::release_action";
            Throw* toss(source.get_attribute(Atoss));
            std::deque<Prop*>& propQue(fsm.get_attribute(props_));
            if(!propQue.empty()){
                Prop* prop(*propQue.begin());
                if(nullptr != toss && nullptr != prop){
                    prop->Toss(*toss);
                }
                propQue.pop_front();
            }
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        toss_entry_action,
        no_action,
        attributes_ << Atoss,
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
            Prop* prop(evt.get_attribute(Aprop));
            if(nullptr != prop){
                DebugOut() << "HandStateMachine::catch_entry_action - " << prop->toString();
                state.get_attribute(Aprop) = prop;
            }
        }
    };

    BOOST_MSM_EUML_ACTION(catch_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "HandStateMachine::catch_exit_action";
        }
    };

    BOOST_MSM_EUML_ACTION(caught_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Prop* prop(source.get_attribute(Aprop));
            if(nullptr != prop && !prop->isDropped()){
                DebugOut() << "HandStateMachine::caugh_action - " << prop->toString();
                prop->Caught();
                fsm.get_attribute(props_).push_front(prop);
            }
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
            DebugOut() << "HandStateMachine::vacant_entry_action";
        }
    };

     BOOST_MSM_EUML_ACTION(vacant_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "HandStateMachine::vacant_exit_action";
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
            DebugOut() << "HandStateMachine::pickup_action";
            Prop* prop = evt.get_attribute(Aprop);
            if(nullptr != prop){
                fsm.get_attribute(props_).push_back(prop);
                prop->Pickup(fsm.get_attribute(Ahand));
            }
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
            std::deque<Prop*>& propQue(fsm.get_attribute(props_));
            DebugOut() << "HandStateMachine::vacant_guard: returned " << std::boolalpha << propQue.empty();
            return propQue.empty();
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
                TOSS + releaseEvent / release_action,
                TOSS [vacant_guard]                     == VACANT,
                VACANT + catchEvent                     == CATCH,
                VACANT [!vacant_guard]                  == DWELL,
                CATCH + caughtEvent / caught_action     == DWELL,
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
            attributes_ << props_ << Ahand, // the attributes
            configure_ << no_configure_, // configuration parameters (flags and funcitons)
            invalid_state_transistion    // default action if transition is invalid
        ), hand_state_machine );

    // the type for the state machine

    typedef msm::back::state_machine<hand_state_machine> Base;

    const char* stateNames[] = {
        "Vacant",
        "Dwell",
        "Toss",
        "Catch",
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
{
}


Hand::~Hand(void)
{
}

bool Hand::isVacant()
{
    return stateMachine_->is_flag_active<vacant_flag__helper>();
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

const char* Hand::getStateName()
{
    return  stateNames[*(stateMachine_->current_state())];
}


void Hand::Pickup(Prop* prop)
{
    assert(nullptr != prop);
    stateMachine_->process_event(pickupEvent(prop));
}


void Hand::Toss(Throw* toss)
{
    DebugOut() << "Hand::Toss - " << std::endl << toString();
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent(toss));
}

void Hand::Release()
{
    DebugOut() << "Hand::Release - " << std::endl << toString() << "Releasing: " << (*stateMachine_->get_attribute(props_).begin())->toString();
    stateMachine_->process_event(releaseEvent);
}

void Hand::Catch(Prop* prop)
{
    DebugOut() << "Hand::Catch - " << std::endl << toString() << "Catching: " << prop->toString();
    if(nullptr != prop){
        stateMachine_->process_event(catchEvent(prop));
    }

}

void Hand::Caught(Prop* prop)
{
    DebugOut() << "Hand::Caught - " << std::endl << toString() << "Caught: " << prop->toString();
    if(nullptr != prop && !prop->isDropped()){
        stateMachine_->process_event(caughtEvent);
    }
}

void Hand::Collision(Prop* prop)
{
    assert(nullptr != prop);
    DebugOut() << "Hand::Collision - " << std::endl << toString() << "Collision with: " << prop->toString();
    stateMachine_->process_event(StateMachine::collisionEvent(prop));
}

std::string Hand::toString()
{
    std::stringstream out;
    out << "Hand id: " << id_ << " State: " << getStateName() << std::endl;
    std::deque<Prop*>& props(stateMachine_->get_attribute(props_));
    for(Prop* p : props){
        out << "    " <<  p->toString() << std::endl;
    }
    return out.str();
}





