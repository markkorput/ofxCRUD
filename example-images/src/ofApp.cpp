//--------------------------------------------------------------
// ofApp.h
//--------------------------------------------------------------

#include "ofMain.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void draw();

private: // attributes

};


//--------------------------------------------------------------
// ofApp.cpp
//--------------------------------------------------------------

void ofApp::setup(){
    // window
    ofSetWindowTitle("ofxCRUD - example-images");
}

void ofApp::draw(){
}

//--------------------------------------------------------------
// main.cpp
//--------------------------------------------------------------

int main(){
    ofSetupOpenGL(1024,768,OF_WINDOW);
    ofRunApp(new ofApp());
}
