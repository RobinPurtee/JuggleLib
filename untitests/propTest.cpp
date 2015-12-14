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
            prop.Tick();
            while(0 < siteswap)
            {
                Assert::IsFalse(responder.id_error);
                Assert::IsFalse(responder.has_caught);
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

            prop.Catch();

        }
        

    };
}