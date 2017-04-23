//--------------------------------------------------------------
// ofApp.h
//--------------------------------------------------------------

#include "ofMain.h"
#include "ofxCRUD.h"

#include "ImageNode.h"

class ofApp : public ofBaseApp{

public:
    void setup();
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
    ImageNode::registerCrud(crudManager);
}

void ofApp::draw(){
    auto resource = crudManager.getResourceDefinition<ImageNode>("ImageNode");
    auto instances = resource->getInstances();
    for(auto imageNodeRef : instances){
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
