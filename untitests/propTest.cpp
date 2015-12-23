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
        static const int test_id = 0xdeadbeef;

        class PropResponder 
        {
        public:
            PropResponder()
            : has_tossed(false)
            , has_caught(false)
            , has_dropped(false)
            , id_error(false)
            {}


            void Tossed(int id_)
            {
                has_tossed = true;
                id_error = id_ != test_id;
            }

            void Catch(Prop* prop)
            {
                has_caught = true;
                id_error = nullptr == prop;
                if(!id_error){
                    DebugOut("In Catch state: %s", prop->getStateName());
                }
            }

            void Dropped(int id_)
            {
                has_dropped = true;
                id_error = id_ != test_id;
            }

            bool has_tossed;
            bool has_caught;
            bool has_dropped;
            bool id_error;


        };


        void connect_prop_responder(Prop& prop_, PropResponder* responder_)
        {
            prop_.ConnectToToss(std::bind(&PropResponder::Tossed, responder_, std::placeholders::_1));
            prop_.ConnectToDrop(std::bind(&PropResponder::Dropped, responder_, std::placeholders::_1));
            prop_.ConnectToCatch(std::bind(&PropResponder::Catch, responder_, std::placeholders::_1));
        }


        void run_til_catch(Prop& prop, PropResponder& responder)
        {
            Hand hand(0);
            int siteswap(3);
            Throw pass(siteswap, &hand);

            prop.Pickup();
            prop.Toss(&pass);
            Assert::IsFalse(responder.id_error);
            Assert::IsTrue(responder.has_tossed);
            while(0 < siteswap)
            {
                Assert::IsFalse(responder.id_error);
                Assert::IsFalse(responder.has_caught, _T("The Prop trigger the catch notification early"));
                prop.Tick();
                --siteswap;
            }
        }


    public:
        
        TEST_METHOD(test_drop)
        {
            PropResponder responder;
            Prop prop(test_id);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Tick();
            Assert::IsFalse(responder.id_error);
            Assert::IsTrue(responder.has_dropped);
        }

        TEST_METHOD(test_catch)
        {
            PropResponder responder;
            Prop prop(test_id);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Catch();
            Assert::IsFalse(responder.id_error);
            Assert::IsFalse(responder.has_dropped);
        }

        TEST_METHOD(invalid_transition_test)
        {
            PropResponder responder;
            Prop prop(test_id);

            connect_prop_responder(prop, &responder);
  
            Assert::IsTrue(prop.isDropped(), _T("The did not start on the ground"));
            prop.Catch();
            Assert::IsTrue(prop.isDropped(), _T("The Prop was caught from the ground which is wrong"));
        }

        TEST_METHOD(dropped_once_caught_test)
        {
            PropResponder responder;
            Prop prop(test_id);
            Hand hand(0);
            int siteswap(1);
            Throw pass(siteswap, &hand);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            DebugOut(_T("After Pickup: %s\n"), prop.getStateName());
            Assert::IsTrue(Prop::State::DWELL == prop.getState(), _T("The state is not DWELL after Pickup"));
            prop.Toss(&pass);
            DebugOut(_T("After Toss: %s\n"), prop.getStateName());

            Assert::IsTrue(responder.has_tossed, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Tick();
            DebugOut(_T("After Tick: %s\n"), prop.getStateName());
            Assert::IsFalse(prop.isInFlight(), _T("Prop is still Flight when it should be in Catch"));
            Assert::IsTrue(responder.has_caught, _T("Prop has not notified of coming catch"));
            prop.Catch();
            DebugOut(_T("After first Catch: %s\n"), prop.getStateName());
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), _T("Prop is not in CATCH state after first Catch call"));
            prop.Catch();
            DebugOut(_T("After Second Catch: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_caught, _T("The Catch did not trigger the catch notification"));
            Assert::IsFalse(prop.isDropped(), _T("Prop has dropped during catch"));
            Assert::IsFalse(prop.isInFlight(), _T("Prop thinks it is still in flight when should be Dwell"));
            prop.Collision();
            DebugOut(_T("After Second Catch: %s\n"), prop.getStateName());
            Assert::IsTrue(responder.has_dropped, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by the collision"));
        }

        TEST_METHOD(dropped_on_toss_test)
        {
            PropResponder responder;
            Prop prop(test_id);
            Hand hand(0);
            int siteswap(1);
            Throw pass(siteswap, &hand);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            prop.Toss(&pass);
            Assert::IsTrue(responder.has_tossed, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Collision();
            Assert::IsTrue(responder.has_dropped, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by the collision"));
        }

        TEST_METHOD(missed_catch_test)
        {
            PropResponder responder;
            Prop prop(test_id);
            Hand hand(0);
            int siteswap(1);
            Throw pass(siteswap, &hand);

            connect_prop_responder(prop, &responder);

            prop.Pickup();
            prop.Toss(&pass);
            Assert::IsTrue(responder.has_tossed, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Tick();
            Assert::IsFalse(prop.isInFlight(), _T("Prop is still Flight when it should be in Catch"));
            Assert::IsTrue(responder.has_caught, _T("Prop has not notified of coming catch"));
            prop.Tick();
            Assert::IsTrue(responder.has_dropped, _T("Prop has not sent the drop notification"));
            Assert::IsTrue(prop.isDropped(), _T("The Prop was not dropped by missing the catch"));
        }

    };
}