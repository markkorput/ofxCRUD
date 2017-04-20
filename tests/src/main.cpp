#include "ofxUnitTests.h"
#include "ofxCRUD.h"

#define TEST_START(x) {ofLog()<<"x";
#define TEST_END }

class ofApp: public ofxUnitTestsApp{
  void run(){
      TEST_START(nothin specific)
      TEST_END
  }
};


#include "ofAppNoWindow.h"
#include "ofAppRunner.h"

//========================================================================
int main( ){
  ofInit();
  auto window = std::make_shared<ofAppNoWindow>();
  auto app = std::make_shared<ofApp>();
  ofRunApp(window, app);
  return ofRunMainLoop();
}
