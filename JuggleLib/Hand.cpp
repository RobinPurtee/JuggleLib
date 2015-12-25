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
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(std::deque<Prop*>, props_);


    BOOST_MSM_EUML_EVENT(releaseEvent)
    BOOST_MSM_EUML_EVENT(caughtEvent)


        /*
        *  toss state and actions
        */

        BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* evtToss(evt.get_attribute(Atoss));
            if(nullptr != evtToss){
                target.get_attribute(Atoss) = evtToss;
            }
        }
    };


    BOOST_MSM_EUML_ACTION(release_action)
    {
        //template <class FSM, class EVT, class State>
        //void operator()(EVT const& evt, FSM& fsm, State& state)
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* toss(source.get_attribute(Atoss));
            std::deque<Prop*>& propQue(fsm.get_attribute(props_));
            if(!propQue.empty())
            {
                Prop* prop(*propQue.begin());
                if(nullptr != toss && nullptr != prop){
                    prop->Toss(toss);
                }
                propQue.pop_front();
            }
        }
    };

    BOOST_MSM_EUML_STATE(
        (
            no_action,
        no_action,
            attributes_ << Atoss,
            configure_ << no_configure_
        ), TOSS)
        /**
        * catch state and actions
        */
        BOOST_MSM_EUML_ACTION(catch_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Prop* prop(evt.get_attribute(Aprop));
            if(nullptr != prop){
                prop->Catch();
                target.get_attribute(Aprop) = prop;
            }
        }
    };

    BOOST_MSM_EUML_ACTION(catch_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            Prop* prop(state.get_attribute(Aprop));
            if(nullptr != prop){
                if(!prop-isDropped()){
                    prop->Catch();
                    fsm.get_attribute(props_).push_front(prop);
                }
                else{

                }
            }
        }
    };

    BOOST_MSM_EUML_STATE(
        (
            no_action,
            catch_exit_action,
            attributes_ << Aprop,
            configure_ << no_configure_
        ), CATCH)


        BOOST_MSM_EUML_STATE(
        (
            no_action,
            no_action,
            attributes_ << no_attributes_,
            configure_ << vacant_flag_
        ), VACANT)


    BOOST_MSM_EUML_ACTION(pickup_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Prop* prop = evt.get_attribute(Aprop);
            if(nullptr != prop){
                fsm.get_attribute(props_).push_back(prop);
                prop->Pickup();
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
            return propQue.empty();
        }
    };

    BOOST_MSM_EUML_TRANSITION_TABLE
        (
            (
                VACANT + pickupEvent / pickup_action    == DWELL,
                DWELL + tossEvent / toss_action         == TOSS,
                TOSS + releaseEvent /release_action,
                TOSS [vacant_guard]                     == VACANT,
                VACANT + catchEvent / catch_action      == CATCH,
                CATCH + caughtEvent                     == DWELL,
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
            attributes_ << Aid << props_, // the attributes
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
    HandStateMachine(int id)
        : Base()
    {
        get_attribute(StateMachine::Aid) = id;
    }
};


Hand::Hand(int id)
    : stateMachine_(new HandStateMachine(id))
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
    assert(nullptr != prop);
    stateMachine_->process_event(StateMachine::pickupEvent(prop));
}


void Hand::Toss(Throw* toss)
{
    assert(nullptr != toss);
    stateMachine_->process_event(StateMachine::tossEvent(toss));
}

void Hand::Release()
{
    stateMachine_->process_event(releaseEvent);
}

void Hand::Catch(Prop* prop)
{
    assert(nullptr != prop);
    stateMachine_->process_event(StateMachine::catchEvent(prop));
}

void Hand::caught(Prop* prop)
{
    assert(nullptr != prop);
    if(!prop->isDropped()){
        stateMachine_->process_event(StateMachine::caughtEvent);
    }
}




