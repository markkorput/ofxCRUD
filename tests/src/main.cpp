#include "ofxUnitTests.h"
#include "ofxCRUD.h"

#define TEST_START(x) {ofLog()<<#x;
#define TEST_END }

using namespace ofxCRUD;

class ofApp: public ofxUnitTestsApp{

    void run(){
        TEST_START(Parameter::getEvent)
            Parameter<int> number;

            // normal ofParameter behaviour
            number.set(123);
            test_eq(number.get(), 123, "");
            test_eq(number.get(), 123, "");

            // getEvent
            number.getEvent.addListener([](int& Parameter<int> numberParam)){
                // change param value before it respond to the next .get call
                numberParam.set(456);
            });

            test_eq(number.get(), 456, "");
        TEST_END
    }
};


#include "ofAppNoWindow.h"
#include "ofAppRunner.h"

int main( ){
  ofInit();
  auto window = std::make_shared<ofAppNoWindow>();
  auto app = std::make_shared<ofApp>();
  ofRunApp(window, app);
  return ofRunMainLoop();
}
