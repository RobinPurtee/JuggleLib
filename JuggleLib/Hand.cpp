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

    BOOST_MSM_EUML_FLAG(vacant_flag);
    BOOST_MSM_EUML_FLAG(isJugglingFlag);
    BOOST_MSM_EUML_FLAG(isCollectingFlag);


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
        }
    };

    BOOST_MSM_EUML_ACTION(release_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "HandStateMachine::release_action";
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
            DebugOut() << "HandStateMachine::catch_entry_action";
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

    BOOST_MSM_EUML_STATE(
    (
        catch_entry_action,
        catch_exit_action,
        attributes_ << no_attributes_,
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
        configure_ << vacant_flag
    ), VACANT)


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
    * Invalid transistion handler
    */
    BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            fsm.process_event(collisionEvent);
        }
    };

    /// The transition table for the Juggling state machine
    /// This machine goes through the Toss, Vacant, Catch, Dwell cycle
    /// it does not allow for more than 1 club in the hand after the first toss
    BOOST_MSM_EUML_TRANSITION_TABLE(
        (

            TOSS == DWELL + tossEvent,
            VACANT == TOSS + releaseEvent [vacant_guard],
            DWELL == TOSS + releaseEvent [!vacant_guard],
            CATCH == VACANT + catchEvent,
            DWELL == VACANT + pickupEvent,
            DWELL == CATCH + caughtEvent
        ) , juggling_transition_table )
    // Declare the Juggling state machine class
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(
        (
            juggling_transition_table,  // The transition table
            init_ << DWELL,             // The initial State
            no_action,                  // The startup action
            no_action,                  // The exit action
            attributes_  << Ahand, // the attributes
            configure_ << isJugglingFlag, // configuration parameters (flags and funcitons)
            invalid_state_transistion    // default action if transition is invalid
        ), juggling_state_machine )
    // declard the type for the state machine
    typedef msm::back::state_machine<juggling_state_machine> juggling_type;
    // define an instance of a juggling state machine
    static juggling_type JUGGLING;

    /// The transition table for the Collecting state machine 
    BOOST_MSM_EUML_TRANSITION_TABLE(
        (
            TOSS == DWELL + tossEvent,
            VACANT == TOSS + releaseEvent [vacant_guard],
            DWELL == TOSS + releaseEvent [!vacant_guard],
            CATCH == VACANT + catchEvent,
            DWELL == VACANT + pickupEvent,
            CATCH == DWELL + catchEvent,
            DWELL == CATCH + caughtEvent,
            DWELL + pickupEvent
        ), collecting_transition_table )
    // Declare the Collecting state machine class
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(
        (
            collecting_transition_table,  // The transition table
            init_ << VACANT,             // The initial State
            no_action,                  // The startup action
            no_action,                  // The exit action
            attributes_  << Ahand, // the attributes
            configure_ << isCollectingFlag, // configuration parameters (flags and funcitons)
            invalid_state_transistion    // default action if transition is invalid
        ), collecting_state_machine )
    // declard the type for the collecting state machine
    typedef msm::back::state_machine<collecting_state_machine> collecting_type;
    // define an instance of a collecting state machine
    static collecting_type COLLECTING;

    const char* subStateNames[] = {
        "Dwell",
        "Toss",
        "Vacant",
        "Catch"
    };

    BOOST_MSM_EUML_TRANSITION_TABLE(
        (
            VACANT == JUGGLING + collisionEvent,
            VACANT == COLLECTING + collisionEvent,
            COLLECTING == VACANT + pickupEvent,
            COLLECTING == VACANT + catchEvent,
            JUGGLING == COLLECTING + tossEvent
        ), hand_transition_table)


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

    const char* handStateNames[] = {
        "Juggling",
        "Collecting",
        "Vacant",
    };

}

struct Hand::HandStateMachine : public Base
{
    HandStateMachine(Hand* hand)
        : Base()
    {
        get_attribute(StateMachine::Ahand) = hand;
        getJugglingState().get_attribute(StateMachine::Ahand) = hand;
        getCollectingState().get_attribute(StateMachine::Ahand) = hand;
    }

    // is the current state machine juggling
    bool isJuggling()
    {
        return is_flag_active<isJugglingFlag_helper>();
    }

    // is the current state machine juggling
    bool isCollecting()
    {
        return is_flag_active<isCollectingFlag_helper>();
    }


    juggling_type& getJugglingState()
    {
        return get_state<juggling_type&>();
    }

    collecting_type& getCollectingState()
    {
        return get_state<collecting_type&>();
    }

    int getState()
    {
        int ret = *current_state();
        if(isJuggling())
        {
            ret = *(getJugglingState().current_state());
        }
        else if(isCollecting())
        {
            ret = *(getCollectingState().current_state());
        }
        return ret;
    }

    const char* getStateName()
    {
        int value = getState();
        const char* ret(nullptr);
        if(isJuggling() || isCollecting())
        {
            ret = subStateNames[value];
        }
        else 
        {
            ret = handStateNames[value];
        }
        return ret;
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
// test if the Hand is in the Vacant state
bool Hand::isVacant()
{
    return props_.empty();
}
// Get the ID of the hand
int Hand::getId()
{
    return id_;
}
// Get the current state of the hand
Hand::State Hand::getState()
{
    return (static_cast<Hand::State>(*(stateMachine_->current_state())));
}
// Get the text name of the current state
const char* Hand::getStateName()
{
    return  stateMachine_->getStateName();
}

// Pick up a prop and add it to the back of the que
void Hand::Pickup(Prop* prop)
{
    if(nullptr != prop){
        stateMachine_->process_event(pickupEvent(prop));
        props_.push_back(prop);
        prop->Pickup(this);
    }
}
// start the toss of the prop at the front of the que
void Hand::Toss(Throw* toss)
{
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent);
    toss_ = toss;
}
// complete the toss of the prop and release it
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
// PropSlot for the Catch signal coming from the in coming Prop
void Hand::Catch(Prop* prop)
{
    DebugOut() << "Hand::Catch - " << std::endl << toString() << "Catching: " << prop->toString();
    if(nullptr != prop){
        propCatching_ = prop;
        stateMachine_->process_event(catchEvent(prop));
    }
}
// Use to signal back to the prop that it has been caught 
// and add the Prop to the front of the que
void Hand::Caught()
{
    if(nullptr != propCatching_){
        DebugOut() << "Hand::Caught - " << std::endl << toString() << "Caught: " << propCatching_->toString();
        if(!propCatching_->isDropped()){
            stateMachine_->process_event(caughtEvent);
            propCatching_->Caught();
            props_.push_front(propCatching_);
        }
    }
    else{
        DebugOut() << "Hand::Caught - " << std::endl << toString() << "Called but the hand has no Prop to catch";
        }

}
// PropSlot for a signal that the Prop has been dropped
// This will cause all Props currently in the hand to be dropped
void Hand::Collision(Prop* prop)
{
    assert(nullptr != prop);
    DebugOut() << "Hand::Collision - " << std::endl << toString() << "Collision with: " << prop->toString();
    stateMachine_->process_event(StateMachine::collisionEvent(prop));
}
// Get the debug status output string
std::string Hand::toString()
{
    std::stringstream out;
    out << "Hand id: " << id_ << " State: " << getStateName() << std::endl;
    for(Prop* p : props_){
        out << "    " <<  p->toString() << std::endl;
    }
    return out.str();
}





