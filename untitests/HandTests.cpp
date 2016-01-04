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


            DebugOut() << "initial state\n" << hand.toString() << prop.toString();
            // test that the hand is vacant and the prop is on the floor
            Assert::IsTrue(hand.isVacant(), L"The hand did not start out vacant");
            Assert::IsTrue(prop.isDropped(), L"The prop was not on the ground");
            hand.Pickup(&prop);
            DebugOut() << "after pickup\n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand is still vacant after the pick up");
            Assert::IsFalse(prop.isDropped(), L"The prop was not picked up");
            // toss the prop
            hand.Toss(&toss);
            DebugOut() << "after toss\n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand has released the prop early");
            Assert::IsFalse(prop.isInFlight(), L"The prop is in flight early");
            Assert::IsTrue(Hand::State::TOSS  == hand.getState(), L"The hand is not in TOSS state");
            // release the prop
            hand.Release();
            DebugOut() << "after release \n" << hand.toString() << prop.toString();
            Assert::IsTrue(hand.isVacant(), L"The hand is not vacant after the release");
            // clock the flight through
            for(int i = 0 ; i < toss.siteswap ; ++i)
            {
                Assert::IsTrue(hand.isVacant(), L"The hand has not released the prop");
                Assert::IsTrue(prop.isInFlight(), L"The prop is not in flight after the release");
                prop.Tick();
            }

            DebugOut() << "after prop flight \n" << hand.toString() << prop.toString();
            Assert::IsFalse(prop.isInFlight(), L"The prop is in flight when it should be in dwell");
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), L"The Prop is not in the CATCH state");
            Assert::IsTrue(Hand::State::CATCH == hand.getState(), L"The Hand is not in the CATCH state");
            Assert::IsFalse(prop.isDropped(), L"The prop has be dropped when it shoud be in dwell");
            
            hand.Caught();
            DebugOut() << "after prop caught \n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand is vacant when it should have a prop in it DWELL");
            Assert::IsFalse(prop.isInFlight(), L"The prop is in flight when it should be in dwell");
            Assert::IsTrue(Prop::State::DWELL == prop.getState(), L"The Prop is not in the DWELL state at the end");
            Assert::IsTrue(Hand::State::DWELL == hand.getState(), L"The Hand is not in the DWELL state at the end");
            Assert::IsFalse(prop.isDropped(), L"The prop has be dropped when it shoud be in dwell");

            

        }

        TEST_METHOD(DropOnCatchTest)
        {
            Hand hand(0);
            Prop prop(0);
            Throw toss(1, &hand);

            DebugOut() << "initial \n" << hand.toString();
            hand.Pickup(&prop);
            DebugOut() << "after pickup \n" << hand.toString();
            hand.Toss(&toss);
            DebugOut() << "after toss \n" << hand.toString();
            hand.Release();
            DebugOut() << "after release \n" << hand.toString() << prop.toString();
            // clock the flight through
            prop.Tick();
            DebugOut() << "after prop flight \n" << hand.toString() << prop.toString();
            Assert::IsTrue(Hand::State::CATCH == hand.getState(), L"The Hand is not in Catch state");
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), L"The Prop is not in Catch state");
            // force a drop of the prop by ticking the prop clock
            prop.Tick();
            DebugOut() << "after prop dropped \n" << hand.toString();
            Assert::IsTrue(prop.isDropped(), L"The Prop is not in Dropped state");
            hand.Caught();
            DebugOut() << "after hand caught \n" << hand.toString() << prop.toString();
            Assert::IsTrue(hand.isVacant(), L"The hand still thinks it has caught the club");
 
        }

        TEST_METHOD(tTest)
        {
            Hand hand(0);
            Prop prop0(0);
            Prop prop1(1);
            Throw toss(4, &hand);

            hand.Pickup(&prop0);
            DebugOut(L"after first pickup \n  %s", hand.toString());
            Assert::IsFalse(hand.isVacant(), L"hand is still vacant after the pick up");
            Assert::IsFalse(prop0.isDropped(), L"prop0 was not picked up");
            hand.Pickup(&prop1);
            DebugOut(L"after second pickup \n  %s", hand.toString());
            Assert::IsFalse(hand.isVacant(), L"hand is still vacant after the pick up");
            Assert::IsFalse(prop0.isDropped(), L"prop0 was not picked up");

        }

    };
}