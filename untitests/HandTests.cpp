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
            std::shared_ptr<Hand> hand(new Hand(0));
            std::shared_ptr<Prop> prop(new Prop(0));
            std::shared_ptr<Throw> toss(new Throw(4, hand.get()));

            DebugOut(_T("initial state\n    Hand: %s\n    Prop: %s"), hand->getStateName(), prop->getStateName());
            // test that the hand is vacant and the prop is on the floor
            Assert::IsTrue(hand->isVacant(), _T("The hand did not start out vacant"));
            Assert::IsTrue(prop->isDropped(), _T("The prop was not on the ground"));
            hand->Pickup(prop.get());
            DebugOut(_T("after pickup \n    Hand: %s\n    Prop: %s"), hand->getStateName(), prop->getStateName());
            Logger::WriteMessage(hand->getStateName());
            Assert::IsFalse(hand->isVacant(), _T("The hand is still vacant after the pick up"));
            Assert::IsFalse(prop->isDropped(), _T("The prop was not picked up"));
            hand->Toss(toss.get());
            DebugOut(_T("after toss \n    Hand: %s\n    Prop: %s"), hand->getStateName(), prop->getStateName());
            Assert::IsFalse(hand->isVacant(), _T("The hand has released the prop early"));
            Assert::IsFalse(prop->isInFlight(), _T("The prop is in flight early"));
            hand->Release();
            DebugOut(_T("after release \n    Hand: %s\n    Prop: %s"), hand->getStateName(), prop->getStateName());
            for(int i = 0 ; i < toss->siteswap ; ++i)
            {
                Assert::IsTrue(hand->isVacant(), _T("The hand has not released the prop"));
                Assert::IsTrue(prop->isInFlight(), _T("The prop is not in flight after the release"));
                prop->Tick();
            }
            DebugOut(_T("after prop flight \n    Hand: %s\n    Prop: %s"), hand->getStateName(), prop->getStateName());
            Assert::IsFalse(hand->isVacant(), _T("The hand is vacant when it should have a prop in it dwell"));
            Assert::IsFalse(prop->isInFlight(), _T("The prop is in flight when it should be in dwell"));
            Assert::IsFalse(prop->isDropped(), _T("The prop has be dropped when it shoud be in dwell"));





        }

    };
}