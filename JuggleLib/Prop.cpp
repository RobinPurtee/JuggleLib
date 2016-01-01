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
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropPublisher, tossed_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropPublisher, catch_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropPublisher, dropped_);

    BOOST_MSM_EUML_FLAG(isDroppedFlag_);
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);

    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, handAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(caughtEvent, handAttributes)

    template<class FSM>
    void connectHandToFsm(FSM& fsm, Hand* hand)
    { 
        if(nullptr == hand){
            return;
        }
        fsm.get_attribute(Ahand) = hand;
        PropSlot slot(nullptr);
        slot = (std::bind(&Hand::Catch, hand, std::placeholders::_1));
        fsm.get_attribute(catch_).connect(slot);
        slot = (std::bind(&Hand::Collision, hand, std::placeholders::_1));
        fsm.get_attribute(dropped_).connect(slot);
    }

    template<class FSM>
    void disconnectHandFromFsm(FSM& fsm)
    {
        Hand* hand = fsm.get_attribute(Ahand);
        if(nullptr == hand){
            return;
        }
        fsm.get_attribute(Ahand) = nullptr;
        PropSlot slot(nullptr);
        slot = (std::bind(&Hand::Catch, hand, std::placeholders::_1));
        fsm.get_attribute(catch_).disconnect(slot);
        slot = (std::bind(&Hand::Collision, hand, std::placeholders::_1));
        fsm.get_attribute(dropped_).disconnect(slot);
    }


    /* 
     *  The catch state and support methods
     */

    BOOST_MSM_EUML_ACTION(catch_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "PropStateMachine::catch_entry_action";
            if(!fsm.get_attribute(catch_).empty()){
                fsm.get_attribute(catch_)(fsm.get_attribute(prop_));
            }

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
     *  The dropped state and support methods
     */

    BOOST_MSM_EUML_ACTION(dropped_entry)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "PropStateMachine::dropped_entry";
            if(!fsm.get_attribute(dropped_).empty()){
                fsm.get_attribute(dropped_)(fsm.get_attribute(prop_));
            }
            disconnectHandFromFsm(fsm);
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        dropped_entry,
        no_action,
        attributes_ << no_attributes_,
        configure_ << isDroppedFlag_
    ), DROPPED)
 
    BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "PropStateMachine::toss_action";
            Throw* destinationToss(fsm.get_attribute(Atoss));
            Throw* sourceToss(evt.get_attribute(Atoss));
            PropSlot slot(nullptr);
            if(nullptr != destinationToss){
                if(nullptr != sourceToss){
                    *destinationToss = *sourceToss;
                }
                if(nullptr != destinationToss->destination){
                    if(nullptr != fsm.get_attribute(Ahand)){
                        disconnectHandFromFsm(fsm);
                    }
                    connectHandToFsm(fsm, destinationToss->destination);
                }
            }
            if(!fsm.get_attribute(tossed_).empty()){
                fsm.get_attribute(tossed_)(fsm.get_attribute(prop_));
            }
        }
    };


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

    BOOST_MSM_EUML_ACTION(tick_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "PropStateMachine::tick_action";
            Throw* toss(fsm.get_attribute(Atoss));
            --toss->siteswap;            
        }
    };

    BOOST_MSM_EUML_STATE(
    (
        flight_entry_action,
        flight_exit_action,
        attributes_ << no_attributes_ ,
        configure_ << isInFlightFlag_
    ), FLIGHT)

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


    BOOST_MSM_EUML_ACTION(pickup_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "PropStateMachine::pickup_action";
            Hand* hand(evt.get_attribute(Ahand));
            if(nullptr != hand){
                connectHandToFsm(fsm, hand);
            }
        }
    };
    /**
     *  Prop state machine transition table
     */

    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        DWELL == CATCH + caughtEvent                     ,
        DROPPED + pickupEvent / pickup_action == DWELL  ,
        
        FLIGHT == DWELL + tossEvent / toss_action        ,
        FLIGHT + tickEvent [!tick_guard] / tick_action,

        CATCH == FLIGHT [tick_guard]                     ,

        DROPPED == CATCH + tickEvent [tick_guard]        ,
        DROPPED == DWELL + collisionEvent                ,
        DROPPED == FLIGHT + collisionEvent               ,
        DROPPED == CATCH + collisionEvent                ,
        DROPPED + collisionEvent
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
        attributes_<< Aid << prop_ << tossed_ << catch_ << dropped_ << Atoss << Ahand,
        configure_ << no_configure_,
        invalid_state_transistion
    ), prop_state_machine)

    // the type for the state machine

    typedef msm::back::state_machine<prop_state_machine> Base;
    const char* stateNames[] = {
        "Catch",
        "Dropped",
        "Dwell",
        "Flight",
    };

}

/**
 *
 */

struct Prop::PropStateMachine : public Base
{
    PropStateMachine(int id, Prop* prop)
    {
        get_attribute(StateMachine::Aid) = id;
        get_attribute(prop_) = prop;
        get_attribute(Ahand) = nullptr;
    }
};

/**
 *
 */

Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(id, this) )
,   id_(id)
{
    stateMachine_->get_attribute(StateMachine::Atoss) = &toss_;
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
const char* Prop::getStateName()
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
    stateMachine_->get_attribute(tossed_).connect(slot);
}

/**
 *
 */
void Prop::disconnectFromToss(PropSlot slot)
{
    stateMachine_->get_attribute(tossed_).disconnect(slot);
}

/**
 *
 */
void Prop::connectToDrop(PropSlot slot)
{
    stateMachine_->get_attribute(dropped_).connect(slot);
}

/**
 *
 */
void Prop::disconnectFromDrop(PropSlot slot)
{
    stateMachine_->get_attribute(dropped_).disconnect(slot);
}

/**
 *
 */
void Prop::connectToCatch(PropSlot slot)
{
    stateMachine_->get_attribute(catch_).connect(slot);
}

/**
 *
 */
void Prop::disconnectFromCatch(PropSlot slot)
{
    stateMachine_->get_attribute(catch_).disconnect(slot);
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
    stateMachine_->process_event(StateMachine::tossEvent(&toss));
}



/**                                                                                        
 *
 */
void Prop::Caught()
{
#ifdef _DEBUG
    Hand* hand =  stateMachine_->get_attribute(Ahand);
    if(nullptr != hand){
        DebugOut() << "Prop::Caught - " << toString() << "by: " << hand->toString(); 
    }
    else{
        DebugOut() << "Prop::Caught - " << toString() << "by: no hand"; 
    }
#endif // _DEBUG
    if(!isDropped()){        
        stateMachine_->process_event(caughtEvent(hand));
    }
}

/**
 *
 */
void Prop::Collision()
{
    stateMachine_->process_event(StateMachine::collisionEvent(this));
}

/**
 *
 */
void Prop::Pickup(Hand* hand)
{
    stateMachine_->process_event(pickupEvent(hand));
}

/**
 *
 */
void Prop::Tick()
{
    if(!isDropped()){
        stateMachine_->process_event(tickEvent);
    }

}


std::string Prop::toString()
{
    std::stringstream out;
    out << "Prop id: " << std::hex << id_ << " State: " << getStateName() << "(" << (*(stateMachine_->current_state())) << ")";
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

