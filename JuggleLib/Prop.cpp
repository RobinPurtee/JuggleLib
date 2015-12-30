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
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);

    BOOST_MSM_EUML_EVENT(pickupEvent)
    BOOST_MSM_EUML_EVENT(caughtEvent)
    BOOST_MSM_EUML_EVENT(catchEvent)


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
            DebugOut(_T("PropStateMachine::catch_exit_action"));
        }
    };
   
    BOOST_MSM_EUML_STATE(
    (
        catch_entry_action, 
        catch_exit_action
    ), CATCH)

    /* 
     *  The dropped state and support methods
     */

    BOOST_MSM_EUML_ACTION(dropped_entry)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut(_T("PropStateMachine::dropped_entry"));
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
 
    /* 
     *  The flight state and support methods
     */
    BOOST_MSM_EUML_ACTION(flight_entry_action)
    {
        template <class FSM,class EVT,class State>
        void operator()(EVT const& evt ,FSM& fsm, State& state )
        {
            DebugOut(_T("PropStateMachine::flight_entry_action"));
        }
    };

    BOOST_MSM_EUML_ACTION(flight_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut(_T("PropStateMachine::flight_exit_action"));
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

    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        DWELL + tossEvent                       == FLIGHT,
        DWELL + pickupEvent,
        FLIGHT + catchEvent                     == CATCH,
        CATCH + caughtEvent                     == DWELL,
        CATCH + tickEvent                       == DROPPED,
        DROPPED + pickupEvent                   == DWELL,
        DROPPED + collisionEvent ,
        DWELL + collisionEvent                  == DROPPED,
        FLIGHT + collisionEvent                 == DROPPED,
        CATCH + collisionEvent                  == DROPPED
    ), prop_transition_table)

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
    const TCHAR* stateNames[] = {
        TEXT("Dwell"),
        TEXT("Flight"),
        TEXT("Catch"),
        TEXT("Dropped"),
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
};

/**
 *
 */
Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(this, std::bind(&Prop::dropped, this, std::placeholders::_1)) )
,   id_(id)
,   hand_(nullptr)
{
}


/**
 *
 */
Prop::~Prop(void)
{
}

/**
 *
 */
Prop::State Prop::getState()
{
    return static_cast<Prop::State>((*(stateMachine_->current_state())));
}

/**
 *
 */
const TCHAR* Prop::getStateName()
{
    return  stateNames[(*(stateMachine_->current_state()))];
}

/**
 *
 */
bool Prop::isDropped()
{
    return stateMachine_->is_flag_active<isDroppedFlag__helper>();
}

/**
 *
 */
bool Prop::isInFlight()
{
    bool bRet = 0 < toss_.siteswap && stateMachine_->is_flag_active<isInFlightFlag__helper>();
    return bRet;
}
        
/**
 *
 */
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
 *
 */
bool Prop::isIdValid(int id)
{
    return id == id_;
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
    if(nullptr != hand_)
        DebugOut(_T("Prop::Catch(%d) Prop state: %s  Hand state: %s"), hand_->getId(), getStateName(), hand_->getStateName()); 
    if(!isDropped()){        
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
    if(isDropped()){
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
    if(isDropped()){
        return;
    }
    else if(decrementSiteswap()){
        stateMachine_->process_event(catchEvent);
        catch_(this);
    }
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

void Prop::dropped(Prop* prop)
{
    if(this == prop){
        dropped_(this);
        disconnectHand();
    }

}

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

