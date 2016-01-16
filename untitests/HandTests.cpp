#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Prop.h"
#include "Throw.h"
#include "Hand.h"
#include "TestHand.h"
#include "DebugStringStream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittests
{
    TEST_CLASS(HandTests)
    {
        TestHand hand;
    
    public:
        TEST_METHOD_INITIALIZE(HandTestsInit)
        {
            hand.addProp(0);
        } 



        TEST_METHOD(HappyDayTest)
        {
            Throw toss(4, &hand);

            DebugOut() << "initial state\n" << hand.toString();
            // test that the hand is vacant and the prop is on the floor
            Assert::IsTrue(hand.isVacant(), L"The hand did not start out vacant");
            Assert::IsTrue(hand.getProp(0)->isDropped(), L"The prop was not on the ground");
            hand.Pickup(hand.getProp(0));
            DebugOut() << "after pickup\n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand is still vacant after the pick up");
            Assert::IsFalse(hand.getProp(0)->isDropped(), L"The prop was not picked up");
            // toss the prop
            hand.Toss(&toss);
            DebugOut() << "after toss\n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand has released the prop early");
            Assert::IsFalse(hand.getProp(0)->isInFlight(), L"The prop is in flight early");
            Assert::IsTrue(Hand::State::TOSS  == hand.getState(), L"The hand is not in TOSS state");
            // release the prop
            hand.Release();
            DebugOut() << "after release \n" << hand.toString();
            Assert::IsTrue(hand.isVacant(), L"The hand is not vacant after the release");
            // clock the flight through
            for(int i = 0 ; i < toss.getSiteswap() ; ++i)
            {
                Assert::IsTrue(hand.isVacant(), L"The hand has not released the prop");
                Assert::IsTrue(hand.getProp(0)->isInFlight(), L"The prop is not in flight after the release");
                hand.tick();
            }

            DebugOut() << "after prop flight \n" << hand.toString();
            Assert::IsFalse(hand.getProp(0)->isInFlight(), L"The prop is in flight when it should be in dwell");
            hand.setTestMessage("after prop flight");
            hand.assertStates(Hand::State::CATCH, Prop::State::CATCH); 
            Assert::IsFalse(hand.getProp(0)->isDropped(), L"The prop has be dropped when it shoud be in dwell");
            
            hand.Caught();
            DebugOut() << "after prop caught \n" << hand.toString();
            Assert::IsFalse(hand.isVacant(), L"The hand is vacant when it should have a prop in it DWELL");
            Assert::IsFalse(hand.getProp(0)->isInFlight(), L"The prop is in flight when it should be in dwell");
            hand.setTestMessage("after prop caught");
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL); 
            Assert::IsFalse(hand.getProp(0)->isDropped(), L"The prop has be dropped when it shoud be in dwell");
        }

        TEST_METHOD(DropOnCatchTest)
        {
            Throw toss(1, &hand);

            hand.setTestMessage("initial ");
            DebugOut() << hand.toString();
            hand.Pickup(hand.getProp(0));
            hand.Toss(&toss);
            hand.Release();
            // clock the flight through
            hand.tick();
            hand.setTestMessage("after prop flight ");
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::CATCH, Prop::State::CATCH);
            // force a drop of the prop by ticking the prop clock
            hand.tick();
            hand.setTestMessage("after prop dropped");
            DebugOut() <<  hand.toString();
            Assert::IsTrue(hand.getProp(0)->isDropped(), L"The Prop is not in Dropped state");
            hand.Caught();
            hand.setTestMessage("after hand caught ");
            DebugOut() << hand.toString();
            Assert::IsTrue(hand.isVacant(), L"The hand still thinks it has caught the club");
 
        }

        /**
         *  Flash 2 balls and collect them
         */
        TEST_METHOD(Flash2BallsTest)
        {
            Throw toss(4, &hand);

            hand.addProp(1);

            DebugOut() << "initial \n" << hand.toString();
            
            hand.setTestMessage("After pickup of prop(0)");
            hand.Pickup(hand.getProp(0));
            DebugOut() <<  hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL, Prop::State::DROPPED);

            hand.setTestMessage("After pickup of prop(1)");
            hand.Pickup(hand.getProp(1));
            DebugOut() <<  hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL, Prop::State::DWELL);
            hand.assertNumberOfProps(2);

            hand.setTestMessage("Tossed first prop");
            hand.Toss(&toss);
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::TOSS, Prop::State::DWELL, Prop::State::DWELL);
            hand.assertNumberOfProps(2);

            hand.setTestMessage("Release first prop");
            hand.Release();
            DebugOut()  << hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::FLIGHT, Prop::State::DWELL);
            hand.assertNumberOfProps(1);

            hand.setTestMessage("After first tick ");
            hand.tick();
            DebugOut() << hand.toString();
            hand.assertStates( Hand::State::DWELL, Prop::State::FLIGHT, Prop::State::DWELL);
            hand.tick();

            hand.setTestMessage("Tossed second prop");
            hand.Toss(&toss);
            DebugOut() << hand.toString();
            hand.assertStates( Hand::State::TOSS, Prop::State::FLIGHT, Prop::State::DWELL);

            hand.setTestMessage("Release second prop");
            hand.Release();
            DebugOut()  << hand.toString();
            hand.assertStates( Hand::State::VACANT, Prop::State::FLIGHT, Prop::State::FLIGHT);
            hand.assertNumberOfProps(0);

            hand.tick();
            hand.tick();
            hand.setTestMessage("4 ticks into it");
            DebugOut() << hand.toString();
            hand.assertStates( Hand::State::CATCH, Prop::State::CATCH, Prop::State::FLIGHT);
            hand.setTestMessage("Hand caught prop 0");
            hand.Caught();
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL, Prop::State::FLIGHT);
            hand.assertNumberOfProps(1);
            hand.Collect();
            hand.tick();
            hand.tick();
            hand.setTestMessage("6 ticks into it");
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::CATCH, Prop::State::DWELL, Prop::State::CATCH);

            hand.setTestMessage("After caught the second prop");
            hand.Caught();
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL, Prop::State::DWELL);
            hand.assertNumberOfProps(2);
        }

        /**
         * Toss one ball from a hand with two and do not set Collect mode before catch
         * this should cause the prop to be dropped but retain the current ball and hand state
         */
        TEST_METHOD(Flash2BallsNoCollectTest)
        {
            Throw toss(1, &hand);

            hand.addProp(1);

            hand.Pickup(hand.getProp(0));
            hand.Pickup(hand.getProp(1));
            hand.Toss(&toss);
            hand.Release();
            hand.setTestMessage("after tick");
            hand.tick();
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DROPPED, Prop::State::DWELL);
            hand.assertNumberOfProps(1);
    
            hand.Caught();
            hand.setTestMessage("After caught prop");
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::DWELL, Prop::State::DROPPED, Prop::State::DWELL);
        }

        TEST_METHOD(TossFromVacant)
        {
            Throw toss(1, &hand);

            hand.setTestMessage("Try to toss from a vacant hand");
            hand.Toss(&toss);
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::VACANT, Prop::State::DROPPED);
            hand.setTestMessage("Try to release after an invalid throw");
            hand.Release();
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::VACANT, Prop::State::DROPPED);
        }
        
        TEST_METHOD(DropFromCatchBeforeCaught)
        {
            hand.setTestMessage("catch a club in vacant hand");
            hand.Catch(hand.getProp(0));
            hand.assertHandState(Hand::State::CATCH);
            hand.setTestMessage("drop during the CATCH state");
            hand.Drop();
            hand.assertStates(Hand::State::VACANT, Prop::State::DROPPED);
            hand.setTestMessage("after dropping the club, signal a caught");
            hand.assertNumberOfProps(0);
            hand.Caught();
            hand.assertStates(Hand::State::VACANT, Prop::State::DROPPED);
            hand.assertNumberOfProps(0);
        }


        TEST_METHOD(PickupThenDrop)
        {
            hand.setTestMessage("Pickup prop");
            hand.Pickup(hand.getProp(0));
            hand.assertStates(Hand::State::DWELL, Prop::State::DWELL);
            hand.assertNumberOfProps(1);
            hand.setTestMessage("Drop after Pickup");
            hand.Drop();
            DebugOut() << hand.toString();
            hand.assertStates(Hand::State::VACANT, Prop::State::DROPPED);
            hand.assertNumberOfProps(0);
        }

    };
}                                                      