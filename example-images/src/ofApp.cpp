//--------------------------------------------------------------
// ofApp.h
//--------------------------------------------------------------

#include "ofMain.h"
#include "ofxCRUD.h"

#include "ImageNode.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

private: // attributes
    ofxCRUD::Manager crudManager;
};


//--------------------------------------------------------------
// ofApp.cpp
//--------------------------------------------------------------

void ofApp::setup(){
    // window
    ofSetWindowTitle("ofxCRUD - example-images");

    // register CRUD resource(s)
    ImageNode::createResource(crudManager);

    crudManager.setupOscReceiver(8080);
}

void ofApp::update(){
    crudManager.update();
}

void ofApp::draw(){
    for(auto imageNodeRef : crudManager.getResource<ImageNode>("ImageNode")->getInstances()){
        imageNodeRef.second->draw();
    }
}

//--------------------------------------------------------------
// main.cpp
//--------------------------------------------------------------

int main(){
    ofSetupOpenGL(1024,768,OF_WINDOW);
    ofRunApp(new ofApp());
}
