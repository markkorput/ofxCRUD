#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
// local
#include "LambdaEvent.h"

#include "ResourceDefinition.h"

namespace ofxCRUD {

    class Manager {

    public:

        Manager() : activeInstanceRef(nullptr){
        }

        template<typename T>
        shared_ptr<ResourceDefinition<T>> defineResource(std::function<void (ResourceDefinition<T>&)>  func);

        const std::vector<shared_ptr<BaseResourceDefinition>>& getResourceDefinitions() const {
            return definedResourceDefinitions;
        }

        shared_ptr<BaseResourceDefinition> getResourceDefinition(const string& resourceType){
            for(auto resDefRef :  definedResourceDefinitions){
                if(resDefRef->getResourceType() == resourceType){
                    return resDefRef;
                }
            }

            return nullptr;
        }

        void process(ofxOscMessage& msg){
            vector<string> parts = ofSplitString(msg.getAddress(), "/", true /*ignoreEmpty*/);

            // is this an ofxCRUD message?
            if(parts.size() < 4 || parts[0] != "ofxCRUD")
                return;

            // which resource are we dealing with
            for(auto resDefRef : definedResourceDefinitions){
                if(parts[1] != resDefRef->getResourceType())
                    continue;

                if(parts[2] == "update"){
                    // "/ofxCRUD/ImageNode/update/1/status", "new_value"

                    if(parts.size() < 5){
                        ofLogWarning() << "instance ID and/or property name missing from update message: " << msg.getAddress();
                        return;
                    }

                    resDefRef->update(ofToInt(parts[3]), parts[4], msg.getArgAsString(0));
                    return;
                }

                if(parts[2] == "create"){
                    // the create instruction also takes the ID of the new instance,
                    // otherwise a create instruction would always require a response to
                    // communicate the new ID to the caller. This is more efficient, plus
                    // the caller is in charge of managing data coherently (and thus creating IDs)
                    // "/ofxCRUD/ImageNode/create/1"
                    resDefRef->createInstance(ofToInt(parts[3]));
                    return;
                }

                if(parts[2] == "read"){
                    // request: "/ofxCRUD/ImageNode/read/1/status"
                    // response: "/ofxCRUD/ImageNode/update/1/status"
                    if(parts.size() < 5){
                        ofLogWarning() << "instance ID and/or property name missing from update message: " << msg.getAddress();
                        return;
                    }

                    string value = resDefRef->read(ofToInt(parts[3]), parts[4]);
                    ofLogWarning() << "TODO reply-connection construct";

                    ofxOscMessage replyMsg;
                    replyMsg.setAddress("/ofxCRUD/"+parts[1]+"/update/"+parts[3]+"/"+parts[4]);
                    replyMsg.addStringArg(value);
                    responseMessageEvent.notifyListeners(replyMsg);
                    return;
                }

                if(parts[2] == "delete"){
                    resDefRef->deleteInstance(ofToInt(parts[3]));
                    return;
                }
            }
        }

        ofAbstractParameter* findParameter(ofAbstractParameter &parameter, const string& path)
        {
            ofAbstractParameter *current= &parameter;
            std::vector<std::string> address = ofSplitString(path,"/",true);

            if(address[0] != current->getEscapedName())
                return NULL;

            for(unsigned int i=1; i<address.size(); i++){

                if(current->type()!=typeid(ofParameterGroup).name())
                    return NULL;

                ofAbstractParameter *next = NULL;
                for(auto it = static_cast<ofParameterGroup*>(current)->begin(); it != static_cast<ofParameterGroup*>(current)->end(); it++){
                    if((*it)->getEscapedName() == address[i]){
                        next = (*it).get();
                        break;
                    }
                }

                // not found?
                if(next == NULL)
                    return NULL;

                current = next;
            }
            return current;
        }

        bool updateParameter(ofxOscMessage& msg, ofAbstractParameter& parameter){
            ofAbstractParameter* p = findParameter(parameter, msg.getAddress());
            if(p == NULL)
                return false;

            if(p->type()==typeid(ofParameter<int>).name() && msg.getArgType(0)==OFXOSC_TYPE_INT32){
                p->cast<int>() = msg.getArgAsInt32(0);
                return true;
            }
            if(p->type()==typeid(ofParameter<float>).name() && msg.getArgType(0)==OFXOSC_TYPE_FLOAT){
                p->cast<float>() = msg.getArgAsFloat(0);
                return true;
            }

            if(p->type()==typeid(ofParameter<double>).name() && msg.getArgType(0)==OFXOSC_TYPE_DOUBLE){
                p->cast<double>() = msg.getArgAsDouble(0);
                return true;
            }

            if(msg.getArgType(0)==OFXOSC_TYPE_STRING){
                p->fromString(msg.getArgAsString(0));
                return true;
            }

            if(p->type()==typeid(ofParameter<bool>).name() && (msg.getArgType(0)==OFXOSC_TYPE_TRUE
                || msg.getArgType(0)==OFXOSC_TYPE_FALSE
                || msg.getArgType(0)==OFXOSC_TYPE_INT32
                || msg.getArgType(0)==OFXOSC_TYPE_INT64
                || msg.getArgType(0)==OFXOSC_TYPE_FLOAT
                || msg.getArgType(0)==OFXOSC_TYPE_DOUBLE
                || msg.getArgType(0)==OFXOSC_TYPE_STRING
                || msg.getArgType(0)==OFXOSC_TYPE_SYMBOL)){
                p->cast<bool>() = msg.getArgAsBool(0);
                return true;
            }

            return false;
        }

    public: // events

        LambdaEvent<ofxOscMessage> responseMessageEvent;

    private: // attributes

        std::vector<shared_ptr<BaseResourceDefinition>> definedResourceDefinitions;
        shared_ptr<void> activeInstanceRef;
    };


    template<typename T>
    shared_ptr<ResourceDefinition<T>> Manager::defineResource(std::function<void (ResourceDefinition<T>&)>  func){
        auto resDefRef = make_shared<ResourceDefinition<T>>();
        func(*resDefRef.get());
        definedResourceDefinitions.push_back(resDefRef);
        return resDefRef;
    }
}
