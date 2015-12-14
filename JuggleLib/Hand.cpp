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

    BOOST_MSM_EUML_ACTION(toss_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* evtToss(evt.get_attribute(Atoss));
            if(nullptr != evtToss)
            {
                target.get_attribute(Atoss) = evtToss;
            }
        }
    };


    BOOST_MSM_EUML_ACTION(release_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Throw* toss = source.get_attribute(Atoss);
            Prop* prop = fsm.getPropsHeld().pop_front();
            if(nullptr != toss && nullptr != prop)
            {
                prop->Toss(toss);
            }
        }
    };

    BOOST_MSM_EUML_STATE(
        (
            no_action,
            no_action,
            attributes_ << Atoss,
            configure_ << no_configure_
        ), 
        TOSS)


    BOOST_MSM_EUML_ACTION(catch_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Prop* prop(evt.get_attribute(Aprop));
            if(nullptr != prop)
            {
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
            if(nullptr != prop)
            {
                fsm->getPropsHeld().push_back(prop);
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


    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(releaseEvent, propAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, propAttributes)
    BOOST_MSM_EUML_EVENT(caughtEvent)

 
    BOOST_MSM_EUML_ACTION(pickup_action)
    {
        template <class FSM, class EVT, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
        {
            Prop* prop = evt.get_attribute(Aprop);
            if(nullptr != prop)
            {
                fsm.getPropsHeld().push_back(prop);
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
            return fsm.getPropsHeld().empty();
        }
    };



    class hand_state_machine : public boost::msm::front::state_machine_def<hand_state_machine> 
    {
        public:
            typedef VACANT_helper initial_state;

            hand_state_machine(int id_)
                :   id(id_) 
            { }

            BOOST_MSM_EUML_DECLARE_TRANSITION_TABLE
            (
                (
                    DWELL + tossEvent / toss_action         == TOSS,
                    TOSS + releaseEvent / release_action,                     
                    TOSS + releaseEvent [vacant_guard]      == VACANT,
                    VACANT + catchEvent / catch_action      == CATCH,
                    CATCH + caughtEvent                     == DWELL,
                    VACANT + pickupEvent / pickup_action    == DWELL,
                    DWELL + pickupEvent / pickup_action 
                )
                , transition_table
            )

            std::deque<Prop*>& getPropsHeld() 
            {   
                return propsHeld; 
            } 
            
            bool is_vacant()
            {
                return propsHeld.empty();
            }

            int get_id()    {return id;}


        private:
         std::deque<Prop*> propsHeld;
         int id;
    };




    // the type for the state machine

    typedef msm::back::state_machine<hand_state_machine> Base;

    const TCHAR* stateNames[] = {
        TEXT("Dwell"),
        TEXT("Toss"),
        TEXT("Vacant"),
        TEXT("Catch"),
    };

}

struct Hand::HandStateMachine : public Base
{
    HandStateMachine(int id_)
        : Base(id_)
    {
    }
};


Hand::Hand(int id_)
: stateMachine(new HandStateMachine(id_))
{
}


Hand::~Hand(void)
{
}


void Hand::Pickup(Prop* prop_)
{
    assert(nullptr != prop_);
    stateMachine->process_event(StateMachine::pickupEvent(prop_));
}


void Hand::Toss(Throw* toss_)
{
    assert(nullptr != toss_);
    stateMachine->process_event(StateMachine::tossEvent(toss_));
}

void Hand::Catch(Prop* prop_)
{
    assert(nullptr != prop_);
    stateMachine->process_event(StateMachine::catchEvent(prop_));
}

