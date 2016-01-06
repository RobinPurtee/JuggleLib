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

    BOOST_MSM_EUML_FLAG(isVacantFlag)
    BOOST_MSM_EUML_FLAG(isJugglingFlag)
    BOOST_MSM_EUML_FLAG(isCollectingFlag)


    BOOST_MSM_EUML_EVENT(releaseEvent)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, propAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(catchEvent, propAttributes)
    BOOST_MSM_EUML_EVENT(caughtEvent)
    BOOST_MSM_EUML_EVENT(collectEvent)


    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropSlot, pickupSlot_)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, releaseSlot_)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, caughtSlot_)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, dropSlot_)




    BOOST_MSM_EUML_ACTION(juggling_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "HandStateMachine::juggling_entry_action";
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        juggling_entry_action,
        no_action,
        attributes_ << no_attributes_,
        configure_ << isJugglingFlag
    ), JUGGLING)

    BOOST_MSM_EUML_ACTION(collecting_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "HandStateMachine::collecting_entry_action";
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        collecting_entry_action,
        no_action,
        attributes_ << no_attributes_,
        configure_ << isCollectingFlag
    ),  COLLECTING)



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

    BOOST_MSM_EUML_STATE((toss_entry_action), TOSS)

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

    BOOST_MSM_EUML_STATE((catch_entry_action, catch_exit_action), CATCH)

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
        configure_ << isVacantFlag
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
            fsm.get_attribute(pickupSlot_)(evt.get_attribute(Aprop));
        }
    };

    BOOST_MSM_EUML_ACTION(release_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "HandStateMachine::release_action";
            fsm.get_attribute( releaseSlot_)();
        }
    };

    BOOST_MSM_EUML_ACTION(caught_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "HandStateMachine::caught_action";
            fsm.get_attribute( caughtSlot_)();
        }
    };

    BOOST_MSM_EUML_ACTION(drop_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "HandStateMachine::drop_action";
            fsm.get_attribute( dropSlot_)();
        }
    };


    /**
    *  test if the hand is actually vacant
    */
    BOOST_MSM_EUML_ACTION(is_vacant)
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

    BOOST_MSM_EUML_TRANSITION_TABLE((
            DWELL == VACANT + pickupEvent / pickup_action,
            TOSS == DWELL + tossEvent,
            VACANT == TOSS + releaseEvent / release_action,
            CATCH == VACANT + catchEvent,
            DWELL == VACANT [!is_vacant],
            DWELL == CATCH + caughtEvent / caught_action,
            DWELL + pickupEvent [is_flag_(isCollectingFlag)] / pickup_action,
            DWELL + catchEvent [is_flag_(isCollectingFlag)] / pickup_action,
            VACANT == CATCH + collisionEvent,
            VACANT == DWELL + collisionEvent,
            VACANT == TOSS + collisionEvent,
            // Action state
            JUGGLING == COLLECTING + tossEvent,
            COLLECTING == JUGGLING + collectEvent
        ), hand_transition_table)

        /**
        * Invalid transistion handler
        */
        BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            DebugOut() << "HandStateMachine::invalid_state_transition";
            fsm.get_attribute(Ahand)->Collision(nullptr);
        }
    };



    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(
        (
            hand_transition_table,      // The transition table
            init_ << VACANT << COLLECTING,            // The initial State
            no_action,                  // The startup action
            no_action,                  // The exit action
            attributes_     << Ahand 
                            << pickupSlot_ 
                            << releaseSlot_ 
                            << caughtSlot_ 
                            << dropSlot_, // the attributes
            configure_ << no_configure_, // configuration parameters (flags and funcitons)
            invalid_state_transistion    // default action if transition is invalid
        ), hand_state_machine );

    // the type for the state machine

    typedef msm::back::state_machine<hand_state_machine> Base;

    const char* handStateNames[] = {
        "Vacant",
        "Dwell",
        "Toss",
        "Catch"
    };

}

struct Hand::HandStateMachine : public Base
{
    HandStateMachine(Hand* hand)
        : Base()
    {
        get_attribute(StateMachine::Ahand) = hand;
        get_attribute(pickupSlot_) = std::bind(&Hand::HandStateMachine::pickupAction, this, std::placeholders::_1);
        get_attribute(releaseSlot_) = std::bind(&Hand::HandStateMachine::releaseAction, this);        
        get_attribute(caughtSlot_) = std::bind(&Hand::HandStateMachine::caughtAction, this);        
        get_attribute(dropSlot_) = std::bind(&Hand::HandStateMachine::dropAction, this);        
    }

    /// is the current state machine juggling
    bool isJuggling()
    {
        return is_flag_active<isJugglingFlag_helper>();
    }

    /// is the current state machine juggling
    bool isCollecting()
    {
        return is_flag_active<isCollectingFlag_helper>();
    }
    /// get the int value of the current state
    int getState()
    {
        int ret = *current_state();
        return ret;
    }

    const char* getStateName()
    {
        int value = getState();
        const char* ret(handStateNames[value]);
        return ret;
    }


    void pickupAction(Prop* prop)
    {
        Hand* hand(get_attribute(Ahand));
        if(nullptr != prop){
            hand->props_.push_back(prop);
            prop->Pickup(hand);
        }
    }

    void releaseAction()
    {
        Hand* hand(get_attribute(Ahand));
        if(!hand->props_.empty())
        {
            (*(hand->props_.begin()))->Toss(*hand->toss_);
            hand->props_.pop_front();
            hand->toss_ = nullptr;
        }
    }

    void caughtAction()
    {
        Hand* hand(get_attribute(Ahand));
        hand->propCatching_->Caught();
        hand->props_.push_front(hand->propCatching_);
    }

    void dropAction()
    {
        Hand* hand(get_attribute(Ahand));
        for(Prop* p : hand->props_){
            p->Collision();
        }
        collisionAction(nullptr);
    }

    void collisionAction(Prop*)
    {
        Hand* hand(get_attribute(Ahand));
        hand->props_.clear();
        hand->toss_ = nullptr;
        process_event(collectEvent);;
    }

};

/// initializing constructor
Hand::Hand(int id)
    : stateMachine_(new HandStateMachine(this))
    , id_(id)
    , toss_(nullptr)
    , propCatching_(nullptr)
{
}

/// destructor
Hand::~Hand(void)
{
}
/// test if the Hand is in the Vacant state
bool Hand::isVacant()
{
    return props_.empty();
}
/// Get the ID of the hand
int Hand::getId()
{
    return id_;
}
/// Get the current state of the hand
Hand::State Hand::getState()
{
    return (static_cast<Hand::State>(*(stateMachine_->current_state())));
}
/// Get the text name of the current state
const char* Hand::getStateName()
{
    return  stateMachine_->getStateName();
}
/// Pick up a prop and add it to the back of the que
void Hand::Pickup(Prop* prop)
{
    stateMachine_->process_event(pickupEvent(prop));
} 
/// start the toss of the prop at the front of the que
void Hand::Toss(Throw* toss)
{
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent);
    toss_ = toss;
}
/// complete the toss of the prop and release it
void Hand::Release()
{
    stateMachine_->process_event(releaseEvent);
}
/// PropSlot for the Catch signal coming from the in coming Prop
void Hand::Catch(Prop* prop)
{
    DebugOut() << "Hand::Catch;" << std::endl << toString() << "Catching: " << prop->toString();
    if(nullptr != prop){
        propCatching_ = prop;
        stateMachine_->process_event(catchEvent(prop));
    }
    DebugOut() << std::endl << toString() << "Catching: " << prop->toString();

}
/// Use to signal back to the prop that it has been caught 
/// and add the Prop to the front of the que
void Hand::Caught()
{
    if(nullptr != propCatching_){
        DebugOut() << "Hand::Caught;";
        if(!propCatching_->isDropped()){
            stateMachine_->process_event(caughtEvent);
        }
        DebugOut() << std::endl << toString() << "Caught: " << propCatching_->toString(); 
    }
    else{
        DebugOut() << "Hand::Caught - " << std::endl << toString() << "Called but the hand has no Prop to catch";
        }

}
/// PropSlot for a signal that the Prop has been dropped
/// This will cause all Props currently in the hand to be dropped
void Hand::Collision(Prop* prop)
{
    assert(nullptr != prop);
    DebugOut() << "Hand::Collision - " << std::endl << toString() << "Collision with: " << prop->toString();
    stateMachine_->process_event(StateMachine::collisionEvent(prop));
}
/// set the hand into collection mode
void Hand::Collect()
{
    stateMachine_->process_event(collectEvent);
}

/// Get the debug status output string
std::string Hand::toString()
{
    std::stringstream out;
    out << "Hand id: " << id_;
    if(stateMachine_->isCollecting()){
        out << " is Collecting in ";
    }
    if(stateMachine_->isJuggling()){
        out << " is Juggling in";
    }
    out << " State(" << stateMachine_->getState() << "): " << getStateName();
    out << std::endl;
    for(Prop* p : props_){
        out << "    " <<  p->toString();
    }
    return out.str();
}





