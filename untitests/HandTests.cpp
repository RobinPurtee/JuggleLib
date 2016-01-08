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

        void TestState(int actualState, int expectedState, const char* progress, 
                       const char* actual, const char* expected)
        {
            std::wstringstream message;

            message << progress << actual << " not: " << expected ;
            Assert::IsTrue(expectedState == actualState, message.str().c_str());
        }


        void TestThrow(Throw* toss, Hand& hand, Prop& prop0, Prop& prop1)
        {
            hand.Toss(toss);
            DebugOut() << "After first toss\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::TOSS), 
                      "After first toss the hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::TOSS));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After first toss prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::DWELL));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After first toss prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::DWELL));

            hand.Release();
            DebugOut() << "After first release\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::DWELL), 
                      "After first release hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::DWELL));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "After first release prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::FLIGHT));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After first release prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::DWELL));
        }

        TEST_METHOD(Flash2BallsTest)
        {
            Hand hand(0);
            Prop prop0(0);
            Prop prop1(1);
            Throw toss(4, &hand);

            int expectedStateValue(0);
            boost::signals2::signal<void()> tick;

            tick.connect(std::bind(&Prop::Tick, &prop0));
            tick.connect(std::bind(&Prop::Tick, &prop1));

            DebugOut() << "initial \n" << hand.toString() << prop0.toString() << prop1.toString();
            
            hand.Pickup(&prop0);
            DebugOut() << "After pickup of prop0\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::DWELL), 
                      "After pickup of prop0 the hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::DWELL));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After pickup the prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::DWELL));

            hand.Pickup(&prop1);
            DebugOut() << "After pickup of prop1\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::DWELL), 
                      "After pickup of prop1 the hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::DWELL));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After pickup the prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::DWELL));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After pickup the prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::DWELL));

            TestThrow(&toss, hand, prop0, prop1);

            tick();
            DebugOut() << "After first tick\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::DWELL), 
                      "After first tick hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::DWELL));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "After first tick prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::FLIGHT));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After first tick prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::DWELL));

            tick();
            hand.Toss(&toss);
            DebugOut() << "After second toss\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::TOSS), 
                      "After second toss the hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::TOSS));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "After second toss prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::FLIGHT));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::DWELL), 
                      "After second toss prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::DWELL));

            hand.Release();
            DebugOut() << "After second release\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::VACANT), 
                      "After second release hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::VACANT));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "After second release prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::FLIGHT));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "After second release prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::FLIGHT));

            tick();
            tick();
            DebugOut() << "First Prop throw complete\n" << hand.toString();
            TestState(static_cast<int>(hand.getState()), static_cast<int>(Hand::State::VACANT), 
                      "First Prop throw complete the hand state = ", 
                      hand.getStateName(), 
                      hand.getStateName(Hand::State::VACANT));
            TestState(static_cast<int>(prop0.getState()), static_cast<int>(Prop::State::CATCH), 
                      "First Prop throw complete prop0 state = ", 
                      prop0.getStateName(), 
                      prop0.getStateName(Prop::State::CATCH));
            TestState(static_cast<int>(prop1.getState()), static_cast<int>(Prop::State::FLIGHT), 
                      "First Prop throw complete prop1 state = ", 
                      prop1.getStateName(), 
                      prop1.getStateName(Prop::State::FLIGHT));


   

        }

    };
}