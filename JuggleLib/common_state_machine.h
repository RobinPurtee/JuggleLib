#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/euml/state_grammar.hpp>
#include <boost/msm/back/state_machine.hpp>

namespace msm = boost::msm;
using namespace boost::msm::front::euml;

class Hand;

namespace StateMachine
{

    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(int, siteswap_)   
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(Hand*, destinationHand_)
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(int, id_)

    BOOST_MSM_EUML_ATTRIBUTES((attributes_ << siteswap_ << destinationHand_ ), tossAttributes);
    BOOST_MSM_EUML_EVENT_WITH_ATTRIBUTES(tossEvent, tossAttributes)

    BOOST_MSM_EUML_EVENT(tickEvent)
    BOOST_MSM_EUML_EVENT(catchEvent)
    BOOST_MSM_EUML_EVENT(pickupEvent)
    BOOST_MSM_EUML_EVENT(collisionEvent)


    BOOST_MSM_EUML_STATE((), Dwell)
    BOOST_MSM_EUML_STATE((), Flight)
}
