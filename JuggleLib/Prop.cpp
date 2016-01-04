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
    BOOST_MSM_EUML_FLAG(isInPlayFlag_);
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);

    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, handAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(caughtEvent, handAttributes)
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
    // IN_PLAY transition table
    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        FLIGHT == DWELL + tossEvent,
        DWELL + pickupEvent / pickup_action,
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
        attributes_ << prop_ << tossed_ << catch_ << Atoss << Ahand,
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
            if(!fsm.get_attribute(dropped_).empty()){
                fsm.get_attribute(dropped_)(fsm.get_attribute(prop_));
            }
            disconnectHandFromFsm(IN_PLAY);
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
        DROPPED == IN_PLAY + collisionEvent,
        IN_PLAY == DROPPED + pickupEvent
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
        attributes_<< prop_ << dropped_,
        configure_ << no_configure_,
        invalid_state_transistion
    ), prop_state_machine)

    // the type for the state machine
    typedef msm::back::state_machine<prop_state_machine> Base;


    const char* stateNames[] = {
        "Dwell",
        "Flight",
        "Catch",
        "Dropped",
    };

    const char* propStateNames[] = {
        "In Play",
        "Dropped",        
    };

}

/**
 *
 */
struct Prop::PropStateMachine : public Base
{
    //constructor
    PropStateMachine(Prop* prop)
    {
        get_attribute(prop_) = prop;
        getInPlayState()->get_attribute(prop_) = prop;
        setHand(nullptr);
    }

    // used internally to get the contained state to set attributes on it 
    in_play* getInPlayState()
    {
        return get_state<in_play*>();
    }

    // set the toss pointer
    void setToss(Throw* toss)
    {
        getInPlayState()->get_attribute(StateMachine::Atoss) = toss;
    }

    // get the current hand pointer
    Hand* getHand()
    {
        return getInPlayState()->get_attribute(StateMachine::Ahand);
    }

    // set the hand pointer
    void setHand(Hand* hand)
    {
        getInPlayState()->get_attribute(StateMachine::Ahand) = hand;
    }

    // get the current state value of the propStateMachine
    int getStateValue(){
        return (*(current_state())); 
    }

    // get the current state value of the inPlayStateMachine
    int getInPlayStateValue(){
        return (*(getInPlayState()->current_state()));
    }

    // connect a PropSlot to the Toss signal
    void connectToToss(PropSlot slot)
    {
        getInPlayState()->get_attribute(tossed_).connect(slot);
    }

    // disconnect a PropSlot from the Toss signal
    void disconnectFromToss(PropSlot slot)
    {
        getInPlayState()->get_attribute(tossed_).disconnect(slot);
    }

    // connect a PropSlot to the Dropped signal
    void connectToDrop(PropSlot slot)
    {
        get_attribute(dropped_).connect(slot);
    }

    // disconnect a PropSlot from the Dropped signal
    void disconnectFromDrop(PropSlot slot)
    {
        get_attribute(dropped_).disconnect(slot);
    }

    // connect a PropSlot to the Catch signal
    void connectToCatch(PropSlot slot)
    {
        getInPlayState()->get_attribute(catch_).connect(slot);
    }

    // disconnect a PropSlot from the Catch signal
    void disconnectFromCatch(PropSlot slot)
    {
        getInPlayState()->get_attribute(catch_).disconnect(slot);
    }


    void connectHand(Hand* hand)
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

    void disconnectHand()
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

};

/**
 *
 */

Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(this) )
,   id_(id)
{
    stateMachine_->setToss(&toss_);
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
    int ret(getStateValue());
    if(isInPlay()){
        ret = getSubStateValue();
    }
    return static_cast<Prop::State>(ret);
}

/**
 *
 */
const char* Prop::getStateName()
{
    const char* ret(propStateNames[getStateValue()]);
    if(isInPlay()){
        ret = stateNames[getSubStateValue()];
    }
    return ret; 
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
   
bool Prop::isInPlay()
{
    return stateMachine_->is_flag_active<isInPlayFlag__helper>();
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
    stateMachine_->connectToToss(slot);
}

/**
 *
 */
void Prop::disconnectFromToss(PropSlot slot)
{
    stateMachine_->disconnectFromToss(slot);
}

/**
 *
 */
void Prop::connectToDrop(PropSlot slot)
{
    stateMachine_->connectToDrop(slot);
}

/**
 *
 */
void Prop::disconnectFromDrop(PropSlot slot)
{
    stateMachine_->disconnectFromDrop(slot);
}

/**
 *
 */
void Prop::connectToCatch(PropSlot slot)
{
    stateMachine_->connectToCatch(slot);
}

/**
 *
 */
void Prop::disconnectFromCatch(PropSlot slot)
{
    stateMachine_->connectToCatch(slot);
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
    stateMachine_->process_event(StateMachine::tossEvent(&toss));
}

/**                                                                                        
 *
 */
void Prop::Caught()
{
    Hand* hand(stateMachine_->getHand());
#ifdef _DEBUG
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
    out << "Prop id: " << id_ << " State(" << getStateValue() << "): " << getStateName() << std::endl;
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


