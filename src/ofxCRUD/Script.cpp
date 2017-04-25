// OF
#include "ofMain.h"
// local
#include "Script.h"

void ofxCRUD::startScript(const string& path, const string& stdoutPath, const string& stderrPath){
    string pathToScript = ofToDataPath(path);
    string pathToStdout = ofToDataPath(stdoutPath);
    string pathToStderr = (stderrPath == "" ? "&1" : " " + stderrPath);
    string backgroundProcessPostfix = " &";
    string cmd = "python3 "+pathToScript+" > "+pathToStdout+" 2>"+pathToStderr+backgroundProcessPostfix;
    ofLogWarning() << "ofxCRUD::startScript is implicitly assuming python3 script for now...";
    ofLog() << "ofxCRUD::startScript running command:\n" << cmd;
    string result = ofSystem(cmd);
}
