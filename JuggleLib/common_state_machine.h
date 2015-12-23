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

    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << Atoss ), tossAttributes);
    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << Aprop ), propAttributes);

    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(tossEvent, tossAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(catchEvent, propAttributes)
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(pickupEvent, propAttributes)
    BOOST_MSM_EUML_EVENT(collisionEvent)
    BOOST_MSM_EUML_EVENT(tickEvent)
    BOOST_MSM_EUML_EVENT(caughtEvent)


    BOOST_MSM_EUML_STATE((), DWELL)
}
