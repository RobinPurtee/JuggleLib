#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/euml/state_grammar.hpp>
#include <boost/msm/back/state_machine.hpp>

namespace msm = boost::msm;
using namespace boost::msm::front::euml;

class Hand;
class Throw;

namespace StateMachine
{

    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Throw*, Atoss)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(int, Aid)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Prop*, Aprop)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Hand*, Ahand)

    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << Atoss ), tossAttributes);
    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << Aprop ), propAttributes);
    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << Ahand ), handAttributes);

    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(tossEvent, tossAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(collisionEvent, propAttributes)
    BOOST_MSM_EUML_EVENT(tickEvent)

    BOOST_MSM_EUML_ACTION(dwell_entry_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "entered DWELL state";
        }
    };
    BOOST_MSM_EUML_ACTION(dwell_exit_action)
    {
        template <class Event, class FSM, class STATE>
        void operator()(Event const& evt, FSM& fsm, STATE& state)
        {
            DebugOut() << "exited DWELL state";
        }
    };
    BOOST_MSM_EUML_STATE((dwell_entry_action, dwell_exit_action), DWELL)
}
