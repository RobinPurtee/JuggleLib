#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Prop.h"
#include "Throw.h"
#include "Hand.h"
#include "DebugStringStream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace untitests
{
    TEST_CLASS(HandTests)
    {
    public:

        TEST_METHOD(HappyDayTest)
        {
            Hand hand(0);
            Prop prop(0);
            Throw toss(4, &hand);


            DebugOut(_T("initial state\n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            // test that the hand is vacant and the prop is on the floor
            Assert::IsTrue(hand.isVacant(), _T("The hand did not start out vacant"));
            Assert::IsTrue(prop.isDropped(), _T("The prop was not on the ground"));
            hand.Pickup(&prop);
            DebugOut(_T("after pickup \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsFalse(hand.isVacant(), _T("The hand is still vacant after the pick up"));
            Assert::IsFalse(prop.isDropped(), _T("The prop was not picked up"));
            // toss the prop
            hand.Toss(&toss);
            DebugOut(_T("after toss \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsFalse(hand.isVacant(), _T("The hand has released the prop early"));
            Assert::IsFalse(prop.isInFlight(), _T("The prop is in flight early"));
            Assert::IsTrue(Hand::State::TOSS  == hand.getState(), _T("The hand is not in TOSS state"));
            // release the prop
            hand.Release();
            DebugOut(_T("after release \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            // clock the flight through
            for(int i = 0 ; i < toss.siteswap ; ++i)
            {
                Assert::IsTrue(hand.isVacant(), _T("The hand has not released the prop"));
                Assert::IsTrue(prop.isInFlight(), _T("The prop is not in flight after the release"));
                prop.Tick();
            }

            DebugOut(_T("after prop flight \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsFalse(hand.isVacant(), _T("The hand is vacant when it should have a prop in it CATCH"));
            Assert::IsFalse(prop.isInFlight(), _T("The prop is in flight when it should be in dwell"));
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), _T("The Prop is not in the CATCH state"));
            Assert::IsTrue(Hand::State::CATCH == hand.getState(), _T("The Hand is not in the CATCH state"));
            Assert::IsFalse(prop.isDropped(), _T("The prop has be dropped when it shoud be in dwell"));
            
            hand.caught(&prop);
            DebugOut(_T("after prop caught \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsFalse(hand.isVacant(), _T("The hand is vacant when it should have a prop in it DWELL"));
            Assert::IsFalse(prop.isInFlight(), _T("The prop is in flight when it should be in dwell"));
            Assert::IsTrue(Prop::State::DWELL == prop.getState(), _T("The Prop is not in the DWELL state at the end"));
            Assert::IsTrue(Hand::State::DWELL == hand.getState(), _T("The Hand is not in the DWELL state at the end"));
            Assert::IsFalse(prop.isDropped(), _T("The prop has be dropped when it shoud be in dwell"));

            

        }

        TEST_METHOD(DropOnCatchTest)
        {
            Hand hand(0);
            Prop prop(0);
            Throw toss(1, &hand);

            hand.Pickup(&prop);
            DebugOut(_T("after pickup \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            hand.Toss(&toss);
            DebugOut(_T("after toss \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            hand.Release();
            DebugOut(_T("after release \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            // clock the flight through
            prop.Tick();
            DebugOut(_T("after prop flight \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsTrue(Hand::State::CATCH == hand.getState(), _T("The Hand is not in Catch state"));
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), _T("The Prop is not in Catch state"));
            // force a drop of the prop by ticking the prop clock
            prop.Tick();
            DebugOut(_T("after prop dropped \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsTrue(prop.isDropped(), _T("The Prop is not in Dropped state"));
            hand.caught(&prop);
            DebugOut(_T("after hand caught \n    Hand: %s\n    Prop: %s"), hand.getStateName(), prop.getStateName());
            Assert::IsTrue(hand.isVacant(), _T("The hand still thinks it has caught the club"));
 
        }

    };
}