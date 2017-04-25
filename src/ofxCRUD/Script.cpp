// OF
#include "ofMain.h"
// local
#include "Script.h"

void ofxCRUD::startScript(const string& path){
    auto script = new ofxCRUD::Script();
    script->setScript(path);
    script->start();
    ofxCRUD::Script::singletonInstance = script;
}

void ofxCRUD::stopScript(){
    if(ofxCRUD::Script::singletonInstance != NULL){
        ofxCRUD::Script::singletonInstance->stop();
        delete ofxCRUD::Script::singletonInstance;
        ofxCRUD::Script::singletonInstance = NULL;
    }
}

using namespace ofxCRUD;

Script* Script::singletonInstance = NULL;

void Script::start(){
    if(threaded){
        startThread();
        return;
    }

    startProcess(blocking);
}

void Script::stop(){
    if(!threaded){

        ofLogWarning() << "stopping non-threaded ofxCRUD::Script not implemented yet";
        return;
    }

    if(isThreadRunning()){
        // stopThread();
        ofLog() << "Waiting for ofXCRUD script to end...";
        waitForThread(true /* stop thread */, 5000 /* ait 5 seconds */);

        if(isThreadRunning()){
            string cmd = "Trying kill -9 (" + ofToString(getThreadId()) + ")";
            ofLogWarning() << "Trying: " << cmd;
            ofSystem(cmd);
        }
    }
}

string Script::startProcess(bool blocking){
    string cmd = application
        + " " + ofToDataPath(scriptPath)
        + " > " + ofToDataPath(stdOutPath)
        + " 2>" + (stdErrPath == "" ? "&1" : ofToDataPath(stdErrPath));
    if(!blocking)
        cmd += " &";

    ofLog() << "ofxCRUD::Script running command:\n" << cmd;
    string result = ofSystem(cmd);
    return result;
}

void Script::threadedFunction(){
    string result = startProcess(true /* blocking */);
    ofLog() << "ofxCRUD::Script process ended with output:\n" << result;
}
