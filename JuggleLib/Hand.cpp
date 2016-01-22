#include "pch.h"
#include "JuggleLib.h"
#include "Hand.h"
#include "Throw.h"
#include "Prop.h"
#include "common_state_machine.h"
#include <sstream>

namespace JuggleLib
{
    namespace std = ::std;


    /**
    This object is actually made up of 3 objects
    1. The code in the anonymous namespace - which is the Boost Meta State Machine implemented using the eUML interface
    2. Hand::StateMachine - which is the actual implementation of the Finite State Machine and its action
                            which has a private member of Hand has full access to the Hand class. This is glue that holds it all together
    3. Hand class - is the public interface and containing object of all instance data.

    */


    namespace 
    {
        using namespace CommonStateMachine;

        BOOST_MSM_EUML_FLAG(isVacantFlag)
        BOOST_MSM_EUML_FLAG(isJugglingFlag)
        BOOST_MSM_EUML_FLAG(isCollectingFlag)


        BOOST_MSM_EUML_EVENT(releaseEvent)
        BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, propAttributes)
        BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(catchEvent, propAttributes)
        BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(dropPropEvent, propAttributes)
        BOOST_MSM_EUML_EVENT(caughtEvent)
        BOOST_MSM_EUML_EVENT(collectEvent)
        BOOST_MSM_EUML_EVENT(dropEvent)
    
        BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Hand::StateMachine*, handSM_)
        BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, releaseSlot_)
        BOOST_MSM_EUML_DECLARE_ATTRIBUTE(ActionSlot, caughtSlot_)
        BOOST_MSM_EUML_DECLARE_ATTRIBUTE(PropSlot, dropSlot_)




        BOOST_MSM_EUML_ACTION(juggling_entry_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - entered JUGGLING State";
            }
        };
        BOOST_MSM_EUML_ACTION(juggling_exit_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - exited JUGGLING State";
            }
        };
        BOOST_MSM_EUML_STATE(
        (
            juggling_entry_action,
            juggling_exit_action,
            attributes_ << no_attributes_,
            configure_ << isJugglingFlag
        ), JUGGLING)

        BOOST_MSM_EUML_ACTION(collecting_entry_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - entered COLLECTING State";
            }
        };
        BOOST_MSM_EUML_ACTION(collecting_exit_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - exited COLLECTING State";
            }
        };
        BOOST_MSM_EUML_STATE(
        (
            collecting_entry_action,
            collecting_exit_action,
            attributes_ << no_attributes_,
            configure_ << isCollectingFlag
        ),  COLLECTING)



        /*
         *  toss state and actions
         */

        BOOST_MSM_EUML_ACTION(toss_entry_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - entered TOSS State";
            }
        };
        BOOST_MSM_EUML_ACTION(toss_exit_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - exited TOSS State";
            }
        };
        BOOST_MSM_EUML_STATE((toss_entry_action, toss_exit_action), TOSS)

        /**
        * catch state and actions
        */
        BOOST_MSM_EUML_ACTION(catch_entry_action)
        {
            template <class FSM, class EVT, class State>
            void operator()(EVT const& evt, FSM& fsm, State& state )
            {
                DebugOut() << "Hand - entered CATCH State";
            }
        };
        BOOST_MSM_EUML_ACTION(catch_exit_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - exited CATCH State";
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
                DebugOut() << "Hand - entered VACANT State";
            }
        };
        BOOST_MSM_EUML_ACTION(vacant_exit_action)
        {
            template <class Event, class FSM, class STATE>
            void operator()(Event const& evt, FSM& fsm, STATE& state)
            {
                DebugOut() << "Hand - exited VACANT State";
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
                DebugOut() << "Hand - executed pickup_action";
                fsm.get_attribute(handSM_)->pickupAction(evt.get_attribute(Aprop));
            }
        };
        BOOST_MSM_EUML_ACTION(release_action)
        {
            template <class FSM, class EVT, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
            {
                DebugOut() << "Hand - executed release_action";
                fsm.get_attribute(handSM_)->releaseAction();
            }
        };
        BOOST_MSM_EUML_ACTION(caught_action)
        {
            template <class FSM, class EVT, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
            {
                DebugOut() << "Hand - executed caught_action";
                fsm.get_attribute( handSM_)->caughtAction();
            }
        };
        BOOST_MSM_EUML_ACTION(drop_prop_action)
        {
            template <class FSM, class EVT, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
            {
                DebugOut() << "Hand - executed drop_prop_action";
                fsm.get_attribute(handSM_)->collisionAction(evt.get_attribute(Aprop));
            }
        };
        BOOST_MSM_EUML_ACTION(drop_action)
        {
            template <class FSM, class EVT, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& source, TargetState& target )
            {
                DebugOut() << "Hand - executed drop_action";
                fsm.get_attribute(handSM_)->dropAction();
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
                bool bRet = fsm.get_attribute(handSM_)->isVacant();
                DebugOut() << "Hand is currently " << ((bRet) ? "VACANT" : "has a prop in it");
                return bRet;
            }
        };
                             
        /** 
         *
         */

        BOOST_MSM_EUML_TRANSITION_TABLE((
                DWELL ==    VACANT + pickupEvent / pickup_action,
                            DWELL + pickupEvent [is_flag_(isCollectingFlag)] / pickup_action,
                TOSS ==     DWELL + tossEvent,
                VACANT ==   TOSS + releaseEvent / release_action,
                CATCH ==    VACANT + catchEvent,
                            DWELL + catchEvent [is_flag_(isJugglingFlag)] / drop_prop_action,
                CATCH ==    DWELL + catchEvent [is_flag_(isCollectingFlag)],
                DWELL ==    VACANT [!is_vacant],
                DWELL ==    CATCH + caughtEvent / caught_action,
                VACANT ==   CATCH + dropEvent / drop_action,
                VACANT ==   DWELL + dropEvent / drop_action,
                VACANT ==   TOSS + dropEvent / drop_action,
                            VACANT + dropEvent,
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
                DebugOut() << "Hand has had an invalid state_transition from State: " << 
                    handStateNames[state] <<
                    " caused by Event: " << typeid(e).name();
                fsm.get_attribute(handSM_)->dropAction();
            }
        };



        BOOST_MSM_EUML_DECLARE_STATE_MACHINE(
            (
                hand_transition_table,      // The transition table
                init_ << VACANT << COLLECTING,            // The initial State
                no_action,                  // The startup action
                no_action,                  // The exit action
                attributes_     << handSM_, // the attributes
                configure_ << no_configure_, // configuration parameters (flags and funcitons)
                invalid_state_transistion    // default action if transition is invalid
            ), hand_state_machine );

        // the type for the state machine

        typedef msm::back::state_machine<hand_state_machine> Base;

        const char* handStateNames[] = {
            "Vacant",
            "Dwell",
            "Toss",
            "Catch",
            "Collecting",
            "Juggling"
        };

    }

    struct Hand::StateMachine : public Base
    {
        Hand* handPtr_;

        StateMachine(Hand* hand)
            : Base()
        {
            handPtr_ = hand;
            get_attribute(handSM_) = this;
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

        bool isVacant()
        {
            return handPtr_->props_.empty();
        }
        /// get the int value of the current state
        int getState()
        {
            int ret = *current_state();
            return ret;
        }
        /// get the name of the current state
        const char* getStateName()
        {
            return getStateName(getState());
        }
        /// get the name of the given state
        static const char* getStateName(int state)
        {
            return handStateNames[state];
        }
        /// Pickup action impl
        void pickupAction(Prop* prop)
        {
            if(nullptr != prop){
                handPtr_->props_.push_back(prop);
                prop->Pickup(handPtr_);
            }
        }
        /// release action impl
        void releaseAction()
        {
            if(!handPtr_->props_.empty())
            {
                (*(handPtr_->props_.begin()))->Toss(*handPtr_->toss_);
                handPtr_->props_.pop_front();
                handPtr_->toss_ = nullptr;
            }
        }
        /// caught action impl
        void caughtAction()
        {
            handPtr_->propCatching_->Caught();
            handPtr_->props_.push_front(handPtr_->propCatching_);
        }
        /// hand drop action impl
        void dropAction()
        {
            DropReportPtr drop(new DropReport(DropReport::DropType::HAND_DROPPED, handPtr_->propCatching_, handPtr_));
            for(Prop* p : handPtr_->props_){
                p->Collision(drop);
            }
            handPtr_->props_.clear();
            handPtr_->toss_ = nullptr;
            handPtr_->propCatching_ = nullptr;
            handPtr_->toString();
            if(!isCollecting()){
                process_event(collectEvent);
            }
        }
        /// drop prop action impl
        void collisionAction(Prop* prop)
        {
            if(nullptr != prop){
                DropReportPtr drop(new DropReport(DropReport::DropType::HAND_DROPPED, handPtr_->propCatching_, handPtr_));
                prop->Collision(drop);
            }
        }

    };

    /// initializing constructor
    Hand::Hand(int id)
        : stateMachine_(new Hand::StateMachine(this))
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

    const char* Hand::getStateName(Hand::State state)
    {
        return Hand::StateMachine::getStateName(static_cast<int>(state));
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
        stateMachine_->process_event(CommonStateMachine::tossEvent(toss));
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
        DebugOut() << "Hand::Catch; Catching: " << prop->toString();
        if(nullptr != prop){
            propCatching_ = prop;
            stateMachine_->process_event(catchEvent(prop));
            DebugOut() << std::endl << toString() << "Catching: " << prop->toString();
        }

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
    /// force a drop from the hand
    void Hand::Drop()
    {
        DebugOut() << "Hand::Drop - " << std::endl << toString();
        stateMachine_->process_event(dropEvent);
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

    std::wstring Hand::toWstring()
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> widend;
        return widend.from_bytes(toString());
    }

}


