#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Prop.h"
#include "Throw.h"
#include "Hand.h"

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

            void Catch(int id_)
            {
                has_caught = true;
                id_error = id_ != test_id;
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
            prop_.tossed.connect(std::bind(&PropResponder::Tossed, responder_, std::placeholders::_1));
            prop_.ready_to_be_caught.connect(std::bind(&PropResponder::Catch, responder_, std::placeholders::_1));
            prop_.dropped.connect(std::bind(&PropResponder::Dropped, responder_, std::placeholders::_1));
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
  
            Assert::IsTrue(prop.isDropped());
            prop.Catch();
            Assert::IsTrue(prop.isDropped());
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
            prop.Toss(&pass);
            Assert::IsTrue(responder.has_tossed, _T("Prop did not trick the tossed notification"));
            Assert::IsTrue(prop.isInFlight(), _T("Prop is not in Flight"));
            prop.Tick();
            Assert::IsFalse(prop.isInFlight(), _T("Prop is still Flight when it should be in Catch"));
            Assert::IsTrue(responder.has_caught, _T("Prop has not notified of coming catch"));
            prop.Catch();
            Assert::IsTrue(responder.has_caught, _T("The Catch did not trigger the catch notification"));
            Assert::IsFalse(prop.isDropped(), _T("Prop has dropped during catch"));
            Assert::IsFalse(prop.isInFlight(), _T("Prop thinks it is still in flight when should be Dwell"));
            prop.Collision();
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