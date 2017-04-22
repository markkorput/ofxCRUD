#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

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

            if(parts.size() < 3 || parts[0] != "ofxCRUD")
                return;

            for(auto resDefRef : definedResourceDefinitions){
                if(parts[1] != resDefRef->getResourceType())
                    continue;

                if(parts[2] == "update"){
                    auto instanceVoidRef = resDefRef->find(ofToInt(parts[3]));
                    if(instanceVoidRef == nullptr){
                        ofLogWarning() << "could not find `"<<parts[1]<<"` instance with id: " << parts[3];
                        return;
                    }

                    // activeInstanceRef
                    auto pInstanceParamGroup = &resDefRef->getParameters();
                    ofParameterGroup g1,g2,g3,g4;
                    g1.setName(parts[0]);
                    g2.setName(parts[1]); g1.add(g2);
                    g3.setName(parts[2]); g2.add(g3);

                    // change group name
                    string tmp = pInstanceParamGroup->getName();
                    pInstanceParamGroup->setName(parts[3]);
                    g3.add(*pInstanceParamGroup);

                    updateParameter(msg, g1);

                    // restore
                    pInstanceParamGroup->setName(tmp);
                    g3.remove(*pInstanceParamGroup);
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

    private:
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
