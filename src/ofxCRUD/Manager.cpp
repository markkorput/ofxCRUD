#include "Manager.h"

using namespace ofxCRUD;

void Manager::update(){
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage msg;
        oscReceiver.getNextMessage(msg);
        process(msg);
    }
}

shared_ptr<BaseResource> Manager::getResource(const string& name){
    for(auto resRef :  resources){
        if(resRef->getName() == name){
            return resRef;
        }
    }

    return nullptr;
}

void Manager::process(ofxOscMessage& msg){
    vector<string> parts = ofSplitString(msg.getAddress(), "/", true /*ignoreEmpty*/);

    // is this an ofxCRUD message?
    if(parts.size() < 4 || parts[0] != "ofxCRUD")
        return;

    // which resource are we dealing with
    for(auto resRef : resources){
        if(parts[1] != resRef->getName())
            continue;

        if(parts[2] == "update"){
            // "/ofxCRUD/ImageNode/update/1/status", "new_value"

            if(parts.size() < 5){
                ofLogWarning() << "instance ID and/or property name missing from update message: " << msg.getAddress();
                return;
            }

            resRef->update(ofToInt(parts[3]), parts[4], msg.getArgAsString(0));
            return;
        }

        if(parts[2] == "create"){
            // the create instruction also takes the ID of the new instance,
            // otherwise a create instruction would always require a response to
            // communicate the new ID to the caller. This is more efficient, plus
            // the caller is in charge of managing data coherently (and thus creating IDs)
            // "/ofxCRUD/ImageNode/create/1"
            resRef->createInstance(ofToInt(parts[3]));
            return;
        }

        if(parts[2] == "read"){
            // request: "/ofxCRUD/ImageNode/read/1/status"
            // response: "/ofxCRUD/ImageNode/update/1/status"
            if(parts.size() < 5){
                ofLogWarning() << "instance ID and/or property name missing from update message: " << msg.getAddress();
                return;
            }

            string value = resRef->read(ofToInt(parts[3]), parts[4]);
            ofLogWarning() << "TODO reply-connection construct";

            ofxOscMessage replyMsg;
            replyMsg.setAddress("/ofxCRUD/"+parts[1]+"/update/"+parts[3]+"/"+parts[4]);
            replyMsg.addStringArg(value);
            responseMessageEvent.notifyListeners(replyMsg);
            return;
        }

        if(parts[2] == "delete"){
            resRef->deleteInstance(ofToInt(parts[3]));
            return;
        }
    }
}

void Manager::setupOscReceiver(int port){
    oscReceiver.setup(port);
}
