#include "pch.h"
#include "JuggleLib.h"
#include "common_state_machine.h"
#include "Hand.h"
#include <iostream>
#include <typeinfo>



namespace 
{
    using namespace StateMachine;
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Prop*, prop_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropSlot, drop_);

    BOOST_MSM_EUML_FLAG(isDroppedFlag_);
    BOOST_MSM_EUML_FLAG(isInPlayFlag_);
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);

    BOOST_MSM_EUML_EVENT(pickupEvent)
    BOOST_MSM_EUML_EVENT(caughtEvent)
    BOOST_MSM_EUML_EVENT(catchEvent)

    /**
     * Invalid transistion handler
     * in this case process a collisionEvent to force the Dropped state
     */
    BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            DebugOut() << "PropStateMachine::invald_state_transistion: by event: " << typeid(e).name() << "with PropMachine state: " << state; 
            fsm.process_event(collisionEvent);
        }
    };


    /* 
     *  The catch state and support methods
     */

    BOOST_MSM_EUML_ACTION(catch_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "PropStateMachine::catch_entry_action";
        }
    };
    BOOST_MSM_EUML_ACTION(catch_exit_action)
    {
        template <class FSM, class EVT, class State>
        void operator()(EVT const& evt, FSM& fsm, State& state)
        {
            DebugOut() << "PropStateMachine::catch_exit_action";
        }
    };
   
    BOOST_MSM_EUML_STATE(
    (
        catch_entry_action, 
        catch_exit_action
    ), CATCH)

    /* 
     *  The flight state and support methods
     */
    BOOST_MSM_EUML_ACTION(flight_entry_action)
    {
        template <class FSM,class EVT,class State>
        void operator()(EVT const& evt ,FSM& fsm, State& state )
        {
            DebugOut() << "PropStateMachine::flight_entry_action";
        }
    };

    BOOST_MSM_EUML_ACTION(flight_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "PropStateMachine::flight_exit_action";
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        flight_entry_action,
        flight_exit_action,
        attributes_ << no_attributes_ ,
        configure_ << isInFlightFlag_
    ), FLIGHT)

    /**
     *  Prop state machine transition table
     */
    // IN_PLAY transition table
    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        FLIGHT == DWELL + tossEvent,
        DWELL + pickupEvent,
        CATCH == FLIGHT + catchEvent,
        DWELL == CATCH + caughtEvent,
        CATCH + tickEvent 
    ), prop_in_play_transition_table)

    //The declaration of the IN_PLAY state machine type
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( 
    (
        prop_in_play_transition_table, 
        init_ << DWELL,
        no_action,
        no_action,
        attributes_ << no_attributes_,  //<< prop_ << drop_,
        configure_ << isInPlayFlag_,
        invalid_state_transistion
    ), in_play_state_machine)

    typedef msm::back::state_machine<in_play_state_machine> in_play;

    static in_play  IN_PLAY;


    /* 
     *  The dropped state and support methods
     */

    BOOST_MSM_EUML_ACTION(dropped_entry)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "PropStateMachine::dropped_entry";
            fsm.get_attribute(drop_)(fsm.get_attribute(prop_));
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        dropped_entry,
        no_action,
        attributes_ << no_attributes_,
        configure_ << isDroppedFlag_
    ), DROPPED)
 


    // prop state machine
    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        IN_PLAY == DROPPED + pickupEvent,
        DROPPED == IN_PLAY + collisionEvent
    ), prop_transition_table)



    /**
     * The declaration of the actual state machine type
     */
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( 
    (
        prop_transition_table, 
        init_ << DROPPED,
        no_action,
        no_action,
        attributes_  << prop_ << drop_,
        configure_ << no_configure_,
        invalid_state_transistion
    ), prop_state_machine)

    // the type for the state machine
    typedef msm::back::state_machine<prop_state_machine> Base;


    const char* stateNames[] = {
        "Dropped",
        "Dwell",
        "Flight",
        "Catch",
    };

    const char* propStateNames[] = {
        "Dropped",        
        "In Play",
    };

}

/**
 *
 */
struct Prop::PropStateMachine : public Base
{
    PropStateMachine(Prop* prop, PropSlot drop)
    {
        get_attribute(prop_) = prop;
        get_attribute(drop_) = drop;

    }
    // is the current state Dropped
    bool isDropped()
    {
        return is_flag_active<isDroppedFlag__helper>();
    }
    // is current state Flight
    bool isInFlight()
    {
        return  is_flag_active<isInFlightFlag__helper>();
    }
    // is the current state In Play 
    bool isInPlay()
    {
        return is_flag_active<isInPlayFlag__helper>();
    }
    // Get the current state value
    int getStateValue()
    {
        int ret(*(current_state()));
        if(isInPlay()){
            ret += (*(get_state<in_play&>().current_state()));
        }
        return ret;
    }
    
    const char* getStateName()
    {
        return stateNames[getStateValue()]; 
    }
};

/**
 *   constructor
 */
Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(this, std::bind(&Prop::dropped, this, std::placeholders::_1)) )
,   id_(id)
,   hand_(nullptr)
{
}

/**
 *  destructor
 */
Prop::~Prop(void)
{
}

/**
 *
 */
Prop::State Prop::getState()
{
    return static_cast<Prop::State>(stateMachine_->getStateValue());
}

/**
 *
 */
const char* Prop::getStateName()
{
    return stateMachine_->getStateName(); 
}
// test for Dropped state
bool Prop::isDropped()
{
    return stateMachine_->isDropped();
}
// test for InFlight state
bool Prop::isInFlight()
{
    bool bRet = 0 < toss_.siteswap && stateMachine_->isInFlight();
    return bRet;
}
// test for InPlay state   
bool Prop::isInPlay()
{
    return stateMachine_->isInPlay();
}
// Get the current siteswap value (0 mean not in flight
int Prop::getCurrentSwap()
{
    return toss_.siteswap;
}

/**
 *
 */
void Prop::connectToToss(PropSlot slot)
{
    tossed_.connect(slot);
}

/**
 *
 */
void Prop::disconnectFromToss(PropSlot slot)
{
    tossed_.disconnect(slot);
}

/**
 *
 */
void Prop::connectToDrop(PropSlot slot)
{
    dropped_.connect(slot);
}

/**
 *
 */
void Prop::disconnectFromDrop(PropSlot slot)
{
    dropped_.disconnect(slot);
}

/**
 *
 */
void Prop::connectToCatch(PropSlot slot)
{
    catch_.connect(slot);
}

/**
 *
 */
void Prop::disconnectFromCatch(PropSlot slot)
{
    catch_.disconnect(slot);
}

/**
 *
 */
void Prop::connectToAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot)
{
    connectToToss(tossSlot);
    connectToDrop(dropSlot);
    connectToCatch(propSlot);
}

/**
 *
 */
void Prop::disconnectFromAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot)
{
    disconnectFromToss(tossSlot);
    disconnectFromDrop(dropSlot);
    disconnectFromCatch(propSlot);
}

/**
 * Start accelorating the prop
 * @remarks 
 * A sucessful toss can only be started if the prop has be caught and in the DWELL state
 */
void Prop::Toss(Throw& toss)
{
    toss_ = toss;
    disconnectHand();
    if(nullptr != toss.destination){
        connectHand(toss.destination);
    }
    stateMachine_->process_event(StateMachine::tossEvent);
    if(!tossed_.empty()){
        tossed_(this);
    }
}

/**                                                                                        
 *
 */
void Prop::Caught()
{
#ifdef _DEBUG
    if(nullptr != hand_){
        DebugOut() << "Prop::Catch - " << toString() << "by: " << hand_->toString(); 
    }
#endif // _DEBUG
    if(!stateMachine_->isDropped()){        
        stateMachine_->process_event(caughtEvent);
    }
}

/**
 *
 */
void Prop::Collision()
{
    disconnectHand();
    stateMachine_->process_event(StateMachine::collisionEvent(this));
    dropped_(this);
}

/**
 *
 */
void Prop::Pickup(Hand* hand)
{
    if(stateMachine_->isDropped()){
        connectHand(hand);
        stateMachine_->process_event(pickupEvent);
    }
    else{
        stateMachine_->process_event(collisionEvent);
    }
}

/**
 *
 */
void Prop::Tick()
{
    if(stateMachine_->isDropped()){
        return;
    }
    else if(decrementSiteswap()){
        stateMachine_->process_event(catchEvent);
        catch_(this);
    }
}
// get the current status string
std::string Prop::toString()
{
    std::stringstream out;
    out << "Prop id: " << id_ << " State(" << stateMachine_->getStateValue() << "): " << getStateName() << std::endl;
    return out.str();
}

// Private methods

bool Prop::decrementSiteswap()
{
    bool bRet(0 == toss_.siteswap);
    if(!bRet){
        --toss_.siteswap;
         bRet = (0 == toss_.siteswap);
    }
    return bRet; 
}
// Slot for the dropped state signal from the state machine
void Prop::dropped(Prop* prop)
{
    if(this == prop){
        dropped_(this);
        disconnectHand();
    }

}
// Connect the state signals to the hand
void Prop::connectHand(Hand* hand)
{ 
    if(nullptr == hand){
        return;
    }
    if(nullptr != hand_)
    {
        disconnectHand();
    }
    hand_ = hand;
    PropSlot slot(nullptr);
    slot = (std::bind(&Hand::Catch, hand_, std::placeholders::_1));
    connectToCatch(slot);
    slot = (std::bind(&Hand::Collision, hand_, std::placeholders::_1));
    connectToDrop(slot);
}
// Disconnect the state signals from the hand
void Prop::disconnectHand()
{
    if(nullptr == hand_){
        return;
    }
    PropSlot slot(nullptr);
    slot = (std::bind(&Hand::Catch, hand_, std::placeholders::_1));
    disconnectFromCatch(slot);
    slot = (std::bind(&Hand::Collision, hand_, std::placeholders::_1));
    disconnectFromDrop(slot);
    hand_ = nullptr;
}

