#include "Manager.h"

using namespace ofxCRUD;

void Manager::update(){
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage msg;
        oscReceiver.getNextMessage(msg);
        process(msg);
    }
}

void Manager::destroy(){
    for(auto shutdown_address : shutdown_addresses){
        vector<string> parts = ofSplitString(shutdown_address, "/");
        ofxOscMessage msg;
        msg.setAddress("/ofxCRUD/_ctrl/shutdown");

        ofLog() << "sending shutdown to: " << shutdown_address;
        oscSender.setup(parts[0], ofToInt(parts[1]));
        oscSender.sendMessage(msg);
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
            // request: "/ofxCRUD/ImageNode/read/1/status/<repsonse-host>/<response-port>"
            // response: "/ofxCRUD/ImageNode/update/1/status"
            if(parts.size() < 5){
                ofLogWarning() << "instance ID and/or property name missing from update message: " << msg.getAddress();
                return;
            }

            string value = resRef->read(ofToInt(parts[3]), parts[4]);

            ofxOscMessage replyMsg;
            replyMsg.setAddress("/ofxCRUD/"+parts[1]+"/update/"+parts[3]+"/"+parts[4]);
            replyMsg.addStringArg(value);
            responseMessageEvent.notifyListeners(replyMsg);

            if(parts.size() < 7){
                ofLogWarning() << "no reply address info in osc message address";
                return;
            }

            //ofLog() << "osc-out: " << replyMsg.getAddress() + " with: " + replyMsg.getArgAsString(0);
            oscSender.setup(parts[5], ofToInt(parts[6]));
            oscSender.sendMessage(replyMsg);
            return;
        }

        if(parts[2] == "delete"){
            resRef->deleteInstance(ofToInt(parts[3]));
            return;
        }
    }

    if(parts[1] == "_ctrl"){
        if(parts[2] == "shutdown" && parts.size() >= 5){
            shutdown_addresses.push_back(parts[3]+"/"+parts[4]);
            return;
        }
    }
}

void Manager::setupOscReceiver(int port){
    oscReceiver.setup(port);
}
