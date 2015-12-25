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
    TEST_CLASS(PropTests)
    {
        static const int test_id_ = 0xdeadbeef;

        class PropResponder 
        {
        public:
            PropResponder()
            : has_tossed_(false)
            , has_caught_(false)
            , has_dropped_(false)
            {}


            void Tossed(Prop* prop)
            {
                has_tossed_ = true;
                DebugOut(_T("In Tossed callback: %s"), prop->getStateName());
            }

            void Catch(Prop* prop)
            {
                has_caught_ = true;
                DebugOut(_T("In Catch callback: %s"), prop->getStateName());
            }

            void Dropped(Prop* prop)
            {
                has_dropped_ = true;
                DebugOut(_T("In Dropped callback: %s"), prop->getStateName());
            }

            bool has_tossed_;
            bool has_caught_;
            bool has_dropped_;


        };


        void connect_prop_responder(Prop& prop, PropResponder* responder)
        {
            prop.ConnectToToss(std::bind(&PropResponder::Tossed, responder, std::placeholders::_1));
            prop.ConnectToDrop(std::bind(&PropResponder::Dropped, responder, std::placeholders::_1));
            prop.ConnectToCatch(std::bind(&PropResponder::Catch, responder, std::placeholders::_1));
        }


        void run_til_catch(Prop& prop, PropResponder& responder)
        {
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            prop.Pickup();
            DebugOut(_T("propTest::run_til_catch: After Pickup state: %s"), prop.getStateName()); 
            prop.Toss(&pass);
            DebugOut(_T("propTest::run_til_catch: After Toss state: %s"), prop.getStateName()); 
            Assert::IsTrue(responder.has_tossed_);
            while(0 < siteswap)
            {
                Assert::IsFalse(responder.has_caught_, _T("The Prop trigger the catch notification early"));
                prop.Tick();
                DebugOut(_T("propTest::run_til_catch: After Tick state: %s"), prop.getStateName()); 
                --siteswap;
            }
        }


    public:
        
        TEST_METHOD(test_drop)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Tick();
            DebugOut(_T("propTest::test_drop: After Dropping Tick state: %s"), prop.getStateName()); 
            Assert::IsTrue(responder.has_dropped_, _T("test_drop: The prop was NOT dropped"));
        }

        TEST_METHOD(test_catch)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Catch();
            Assert::IsFalse(responder.has_dropped_, _T("The prop was dropped"));
        }

        TEST_METHOD(invalid_transition_test)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
  
            Assert::IsTrue(prop.isDropped(), _T("The did not start on the ground"));
            prop.Catch();
            Assert::IsTrue(prop.isDropped(), _T("The Prop was caught from the ground which is wrong"));
        }

        TEST_METHOD(dropped_once_caught_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            Hand hand(0);
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            DebugOut(_T("After Pickup: %s\n"), prop.getStateName());
            Assert::IsTrue(Prop::State::DWELL == prop.getState(), _T("The state is not DWELL after Pickup"));
            prop.Toss(&pass);
            DebugOut(_T("After Toss: %s\n"), prop.getStateName());

            Assert::IsTrue(responder.has_tossed_, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Tick();
            DebugOut(_T("After Tick: %s\n"), prop.getStateName());
            Assert::IsFalse(prop.isInFlight(), _T("Prop is still Flight when it should be in Catch"));
            Assert::IsTrue(responder.has_caught_, _T("Prop has not notified of coming catch"));
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), _T("Prop is not in CATCH state after ticked out Catch call"));
            prop.Catch();
            DebugOut(_T("After first Catch: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_caught_, _T("The Catch did not trigger the catch notification"));
            Assert::IsFalse(prop.isDropped(), _T("Prop has dropped during catch"));
            Assert::IsFalse(prop.isInFlight(), _T("Prop thinks it is still in flight when should be Dwell"));
            prop.Collision();
            DebugOut(_T("After Collistion: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_dropped_, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by the collision"));
        }

        TEST_METHOD(dropped_on_toss_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            Hand hand(0);
            int siteswap(1);
            Throw pass(siteswap, &hand);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            prop.Toss(&pass);
            Assert::IsTrue(responder.has_tossed_, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Collision();
            Assert::IsTrue(responder.has_dropped_, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by the collision"));
        }

        TEST_METHOD(missed_catch_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            DebugOut(_T("After Pickup: %s\n"), prop.getStateName());
            prop.Toss(&pass);
            DebugOut(_T("After Toss: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_tossed_, _T("Prop did not tick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Tick();
            DebugOut(_T("After first Tick: %s\n"), prop.getStateName());
            Assert::IsFalse(prop.isInFlight(), _T("Prop is still Flight when it should be in Catch"));
            Assert::IsTrue(responder.has_caught_, _T("Prop has not notified of coming catch"));
            prop.Tick();
            DebugOut(_T("After second Tick: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_dropped_, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by missing the catch"));
        }

    };
}