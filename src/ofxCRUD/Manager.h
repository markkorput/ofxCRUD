#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
// local
#include "LambdaEvent.h"

#include "Resource.h"

namespace ofxCRUD {

    class Manager {

    public:
        ~Manager(){ destroy(); }
        void update();
        void destroy();

        template<typename T>
        shared_ptr<Resource<T>> createResource(std::function<void (Resource<T>&)>  func){
            auto resRef = make_shared<Resource<T>>();
            func(*resRef.get());
            resources.push_back(resRef);
            return resRef;
        }

        const std::vector<shared_ptr<BaseResource>>& getResources() const {
            return resources;
        }

        shared_ptr<BaseResource> getResource(const string& name);

        template<typename ResourceType>
        shared_ptr<Resource<ResourceType>> getResource(const string& resourceType){
            return static_pointer_cast<Resource<ResourceType>>(getResource(resourceType));
        }

        void process(ofxOscMessage& msg);

        void setupOscReceiver(int port);

    public: // events

        LambdaEvent<ofxOscMessage> responseMessageEvent;

    private: // attributes

        std::vector<shared_ptr<BaseResource>> resources;
        ofxOscReceiver oscReceiver;
        ofxOscSender oscSender;

        std::vector<string> shutdown_addresses;
    };
}
