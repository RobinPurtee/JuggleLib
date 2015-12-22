#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Prop.h"
#include "Throw.h"
#include "Hand.h"

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

            std::wstring stateName = hand->getStateName();

            Assert::IsTrue(hand->isVacant(), _T("The hand did not start out vacant"));
            Assert::IsTrue(prop->isDropped(), _T("The prop was not on the ground"));
            hand->Pickup(prop.get());
            stateName = hand->getStateName();
            Assert::IsFalse(hand->isVacant(), _T("The hand is still vacant after the pick up"));
            Assert::IsFalse(prop->isDropped(), _T("The prop was not picked up"));
            hand->Toss(toss.get());
            stateName = hand->getStateName();
            Assert::IsFalse(hand->isVacant(), _T("The hand has released the prop early"));
            Assert::IsFalse(prop->isInFlight(), _T("The prop is in flight early"));
            hand->Release();
            for(int i = 0 ; i < toss->siteswap ; ++i)
            {
                prop->Tick();
                Assert::IsTrue(hand->isVacant(), _T("The hand has not released the prop"));
                Assert::IsTrue(prop->isInFlight(), _T("The prop is not in flight after the release"));
            }
            Assert::IsFalse(hand->isVacant(), _T("The hand is vacant when it should be catching"));
            Assert::IsFalse(prop->isInFlight(), _T("The prop is in flight when it should be in catch"));
            hand->Catch(prop.get());
            Assert::IsFalse(hand->isVacant(), _T("The hand is vacant when it should have a prop in it dwell"));
            Assert::IsFalse(prop->isInFlight(), _T("The prop is in flight when it should be in dwell"));
            Assert::IsFalse(prop->isDropped(), _T("The prop has be dropped when it shoud be in dwell"));





        }

    };
}