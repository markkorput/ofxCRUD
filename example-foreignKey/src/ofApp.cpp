//--------------------------------------------------------------
// ofApp.h
//--------------------------------------------------------------

#include "ofMain.h"
#include "ofxCRUD.h"

class ValueNode {
    public:
        ValueNode() : autoMove(false){
            seed = ofRandom(123.0f);
        }

        void update(){
            if(autoMove)
                value = sin(seed+ofGetElapsedTimef());
        }

        void setValue(float value){ this->value = value; }
        float getValue(){ return value; }
        void setAutoMove(bool value){ autoMove = value; }
    private:
        float seed, value;
        bool autoMove;
};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

private: // attributes
    ofxCRUD::Manager manager;
    shared_ptr<Resource<ValueNode>> valuesNodesManager;
};


//--------------------------------------------------------------
// ofApp.cpp
//--------------------------------------------------------------

void ofApp::setup(){
    // window
    ofSetWindowTitle("ofxCRUD - example-images");

    // register CRUD resource(s)
    manager.createResource<ValueNode>([](Resource<ValueNode>& def){
        def.setName("Node");
        def.addProperty("value",
            [](ValueNode& node){ return ofToString(node.getValue()); },
            [](ValueNode& node, const string& value){ node.setValue(ofToFloat(value)); });
        def.addProperty("autoMove",
            nullptr,
            [](ValueNode& node, const string& value){ node.setAutoMove(ofToBool(value)); });
    });

    valuesNodesManager = manager.getResource<ValueNode>("Node");

    manager.setupOscReceiver(8080);
}

void ofApp::update(){
    manager.update();
    for(auto nodeRef : valuesNodesManager->getInstances()){
        nodeRef.second->update();
    }
}

void ofApp::draw(){
    float y = 10;
    for(auto nodeRef : valuesNodesManager->getInstances()){
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofDrawRectangle(ofGetWidth() * (0.5f - nodeRef.second->getValue()*0.4f), y, 25, 25);
        y += 50;
    }
}

//--------------------------------------------------------------
// main.cpp
//--------------------------------------------------------------

int main(){
    ofSetupOpenGL(1024,768,OF_WINDOW);
    ofRunApp(new ofApp());
}
