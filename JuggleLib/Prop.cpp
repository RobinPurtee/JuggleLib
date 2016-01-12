#include "pch.h"
#include "JuggleLib.h"
#include "common_state_machine.h"
#include "Hand.h"
#include <iostream>
#include <typeinfo>



namespace 
{
    using namespace StateMachine;
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, tick_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, drop_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ThrowSlot, tossedSlot_);
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(HandSlot, pickup_);
 

    BOOST_MSM_EUML_FLAG(isDroppedFlag_);
    BOOST_MSM_EUML_FLAG(isInPlayFlag_);
    BOOST_MSM_EUML_FLAG(isInFlightFlag_);

    BOOST_MSM_EUML_EVENT(catchEvent)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, handAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(caughtEvent, handAttributes)

    /**
     * Invalid transistion handler
     * in this case process a collisionEvent to force the Dropped state
     */
    BOOST_MSM_EUML_ACTION(invalid_state_transistion)
    {
        template <class FSM,class Event>
        void operator()(Event const& e,FSM& fsm,int state)
        {
            DebugOut() << "Prop - invald_state_transistion: by event: " << typeid(e).name() << " with Prop state: " << stateNames[state]; 
            fsm.get_attribute(drop_)();
        }
    };
    /// pickup action
    BOOST_MSM_EUML_ACTION(pickup_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "Prop - pickup_action";
            fsm.get_attribute(pickup_)(evt.get_attribute(Ahand));
        }
    };
    /// toss action
    BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "Prop - toss_action";
            fsm.get_attribute(tossedSlot_)(evt.get_attribute(Atoss));
        }
    };
    /// tick_action
    BOOST_MSM_EUML_ACTION(tick_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "Prop - tick_action";
            fsm.get_attribute(tick_)();
        }
    };
    /// dropped action - this forces the state machine into dropped state
    BOOST_MSM_EUML_ACTION(dropped_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            DebugOut() << "Prop - dropped_action";
            fsm.get_attribute(drop_)();
        }
    };


    ///  The catch state and support methods
    BOOST_MSM_EUML_ACTION(catch_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - entered CATCH state";
        }
    };
    BOOST_MSM_EUML_ACTION(catch_exit_action)
    {
        template <class FSM, class EVT, class State>
        void operator()(EVT const& evt, FSM& fsm, State& state)
        {
            DebugOut() << "Prop - exited CATCH state";
        }
    };
    BOOST_MSM_EUML_STATE((catch_entry_action, catch_exit_action), CATCH)

    /// The flight state and support methods
    BOOST_MSM_EUML_ACTION(flight_entry_action)
    {
        template <class FSM,class EVT,class State>
        void operator()(EVT const& evt ,FSM& fsm, State& state )
        {
            DebugOut() << "Prop - entered FLIGHT state";
        }
    };
    BOOST_MSM_EUML_ACTION(flight_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - exited FLIGHT state";
        }
    };
    BOOST_MSM_EUML_STATE(
    (
        flight_entry_action,
        flight_exit_action,
        attributes_ << no_attributes_ ,
        configure_ << isInFlightFlag_
    ), FLIGHT)

    // IN_PLAY transition table
    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        FLIGHT == DWELL + tossEvent / toss_action,
        DWELL + pickupEvent / pickup_action,
        CATCH == FLIGHT + catchEvent,
        DWELL == CATCH + caughtEvent,
        FLIGHT + tickEvent / tick_action,
        CATCH + tickEvent / dropped_action,
        DWELL + tickEvent
    ), prop_in_play_transition_table)
    // The declaration of the IN_PLAY state machine type
    BOOST_MSM_EUML_ACTION(inplay_entry_action)
    {
        template <class FSM,class EVT,class State>
        void operator()(EVT const& evt ,FSM& fsm, State& state )
        {
            DebugOut() << "Prop - entered IN_PLAY state";
        }
    };
    BOOST_MSM_EUML_ACTION(inplay_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - exited IN_PLAY state";
        }
    };
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( 
    (
        prop_in_play_transition_table, 
        init_ << DWELL,
        inplay_entry_action,
        inplay_exit_action,
        attributes_ << tick_ << pickup_ << tossedSlot_ << drop_,
        configure_ << isInPlayFlag_,
        invalid_state_transistion
    ), in_play_state_machine)

    typedef msm::back::state_machine<in_play_state_machine> in_play_type;

    static in_play_type  IN_PLAY;
    ///  The dropped state and support methods
    BOOST_MSM_EUML_ACTION(dropped_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - entered DROPPED state";
        }
    };
    BOOST_MSM_EUML_ACTION(dropped_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - exited DROPPED state";
        }
    };
    BOOST_MSM_EUML_STATE(
    (
        dropped_entry_action,
        dropped_exit_action,
        attributes_ << no_attributes_,
        configure_ << isDroppedFlag_
    ), DROPPED)
    /// prop Main state machine
    BOOST_MSM_EUML_TRANSITION_TABLE(
    (
        IN_PLAY == DROPPED + pickupEvent / pickup_action,
        DROPPED == IN_PLAY + collisionEvent / dropped_action
    ), prop_transition_table)
    /// The declaration of the actual state machine type
    BOOST_MSM_EUML_ACTION(prop_state_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - entered Main State Machine";
        }
    };
    BOOST_MSM_EUML_ACTION(prop_state_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "Prop - exited Main State Machine";
        }
    };
    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( 
    (
        prop_transition_table, 
        init_ << DROPPED,
        prop_state_entry_action,
        prop_state_exit_action,
        attributes_  << drop_ << pickup_,
        configure_ << no_configure_,
        invalid_state_transistion
    ), prop_state_machine)

    /// the type for the state machine
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
class Prop::PropStateMachine : public Base
{
public:
    PropStateMachine(Prop* prop)
    {
        prop_ = prop;
        get_attribute(pickup_) = std::bind(&Prop::PropStateMachine::pickupAction, this, std::placeholders::_1);
        get_attribute(drop_) = std::bind(&Prop::PropStateMachine::collisionAction, this);
        get_state<in_play_type&>().get_attribute(drop_) = std::bind(&Prop::PropStateMachine::droppedAction, this);
        get_state<in_play_type&>().get_attribute(pickup_) = std::bind(&Prop::PropStateMachine::pickupAction, this, std::placeholders::_1);
        get_state<in_play_type&>().get_attribute(tossedSlot_) = std::bind(&Prop::PropStateMachine::tossedAction, this, std::placeholders::_1);
        get_state<in_play_type&>().get_attribute(tick_) = std::bind(&Prop::PropStateMachine::tickAction, this);        
    }
    /// is the current state Dropped
    bool isDropped()
    {
        return is_flag_active<isDroppedFlag__helper>();
    }
    /// is current state Flight
    bool isInFlight()
    {
        return  is_flag_active<isInFlightFlag__helper>();
    }
    /// is the current state In Play 
    bool isInPlay()
    {
        return is_flag_active<isInPlayFlag__helper>();
    }
    /// Get the current state value
    int getStateValue()
    {
        int ret(*(current_state()));
        if(isInPlay()){
            ret += (*(get_state<in_play_type&>().current_state()));
        }
        return ret;
    }
    /// get the current state string   
    const char* getStateName()
    {
        return getStateName(getStateValue()); 
    }
    /// get the state string for the given state 
    static const char* getStateName(int state)
    {
        return stateNames[state];
    }
    /// Slot for the pickup state signal
    void pickupAction(Hand* hand)
    {
        prop_->connectHand(hand);    
    }   
    /// Slot for the dropped state signal from the state machine
    void droppedAction()
    {
        DebugOut() << "Prop::PropStateMachine::droppedAction";
        process_event(collisionEvent);
    }

    void collisionAction()
    {
        DebugOut() << "Prop::PropStateMachine::collisionAction";
        prop_->dropped_(prop_);
        prop_->disconnectHand();
    }        

    /// Slot for the tossed action
    void tossedAction(Throw* toss)
    {
        prop_->toss_ = *toss;
        prop_->disconnectHand();
        if(nullptr != toss->getDestination()){
            prop_->connectHand(toss->getDestination());
        }
        if(!prop_->tossed_.empty()){
            prop_->tossed_(prop_);
        }        
    }

    void tickAction()
    {
        if(prop_->toss_.decrementSiteswap()){
            process_event(catchEvent);
            prop_->catch_(prop_);
        }
    }



private:
    Prop* prop_;
};

/// Initializing constructor
Prop::Prop(int id)
:   stateMachine_(new Prop::PropStateMachine(this))
,   id_(id)
,   hand_(nullptr)
{
}
///  destructor
Prop::~Prop(void)
{
}
/// Get the current state of the Prop
Prop::State Prop::getState()
{
    return static_cast<Prop::State>(stateMachine_->getStateValue());
}
/// Get the string for the current state
const char* Prop::getStateName()
{
    return stateMachine_->getStateName(); 
}
/// Get the string for the given state
const char* Prop::getStateName(Prop::State state)
{
    return Prop::PropStateMachine::getStateName(static_cast<int>(state)); 
}
/// test for Dropped state
bool Prop::isDropped()
{
    return stateMachine_->isDropped();
}
/// test for InFlight state
bool Prop::isInFlight()
{
    bool bRet = !toss_.isZero() && stateMachine_->isInFlight();
    return bRet;
}
/// test for InPlay state   
bool Prop::isInPlay()
{
    return stateMachine_->isInPlay();
}
/// Connect a slot to the Toss signal
void Prop::connectToToss(PropSlot slot)
{
    tossed_.connect(slot);
}
/// Disconnect a slot from the Toss signal
void Prop::disconnectFromToss(PropSlot slot)
{
    tossed_.disconnect(slot);
}
/// Connect a slot to the Drop signal
void Prop::connectToDrop(PropSlot slot)
{
    dropped_.connect(slot);
}
/// Disconnect a slot from the Drop signal
void Prop::disconnectFromDrop(PropSlot slot)
{
    dropped_.disconnect(slot);
}
/// Connect a slot to the Catch signal
void Prop::connectToCatch(PropSlot slot)
{
    catch_.connect(slot);
}
/// Disconnect a slot from the Catch signal
void Prop::disconnectFromCatch(PropSlot slot)
{
    catch_.disconnect(slot);
}
/// connect all 3 slots
void Prop::connectToAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot)
{
    connectToToss(tossSlot);
    connectToDrop(dropSlot);
    connectToCatch(propSlot);
}
/// disconnect all 3 slots
void Prop::disconnectFromAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot)
{
    disconnectFromToss(tossSlot);
    disconnectFromDrop(dropSlot);
    disconnectFromCatch(propSlot);
}
/// Throw the prop
void Prop::Toss(Throw& toss)
{
    stateMachine_->process_event(StateMachine::tossEvent(&toss));
}
/// Recieved to indedcate that the prop has been caught
void Prop::Caught()
{
#ifdef _DEBUG
    if(nullptr != hand_){
        DebugOut() << "Prop::Catch - " << toString() << "by: " << hand_->toString(); 
    }
#endif // _DEBUG
    //if(!stateMachine_->isDropped()){        
        stateMachine_->process_event(caughtEvent(hand_));
    //}
}

/// drop the prop
void Prop::Collision()
{
    stateMachine_->process_event(StateMachine::collisionEvent(this));
}
/// The prop has been picked up by the given hand
void Prop::Pickup(Hand* hand)
{
    stateMachine_->process_event(pickupEvent(hand));
}
/// handler for a clock tick
void Prop::Tick()
{
    stateMachine_->process_event(tickEvent);
}
// get the current status string
std::string Prop::toString()
{
    std::stringstream out;
    out << "Prop id: " << id_ << " State(" << stateMachine_->getStateValue() << "): " << getStateName() << std::endl;
    return out.str();
}

std::wstring Prop::toWstring()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> widend;
    return widend.from_bytes(toString());
}


// Private methods

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

