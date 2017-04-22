#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

namespace ofxCRUD {

    class Actuator {
    public:
        void setParameters(ofParameterGroup* newParameters){
            params = newParameters;
        }

        ofParameterGroup* getParameters(){
            return params;
        }

    private:
        ofParameterGroup* params;
    };

    class BaseResourceDefinition {
    protected:
        string resourceType;
        ofParameterGroup parameterGroup;

    public:

        BaseResourceDefinition(){
            addProperty<int>("id");
        }

        const string& getResourceType(){
            return resourceType;
        }

        ofParameterGroup& getParameters(){
            return parameterGroup;
        }

        template<typename PropType>
        void addProperty(const string& name){
            auto param = new ofParameter<PropType>();
            param->setName(name);
            parameterGroup.add(*param);
        }

        shared_ptr<void> createInstance(){
            return nullptr;
        }

        virtual shared_ptr<void> find(unsigned int id) = 0;
    };


    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

    public:

        ResourceDefinition() : nextId(1){
        }

        void setResourceType(const string& newResourceType){
            resourceType = newResourceType;
        }

        shared_ptr<ResourceType> createInstance(){
            // create
            auto ref = make_shared<ResourceType>();
            // store
            instances[nextId] = ref;
            nextId++;
            // return
            return ref;
        }

        virtual shared_ptr<void> find(unsigned int id){
            auto it = instances.find(id);
            if(it == instances.end())
                return nullptr;
            return it->second;
        }

        shared_ptr<Actuator> getActuator(shared_ptr<ResourceType>){
            auto actuatorRef = make_shared<Actuator>();
            actuatorRef->setParameters(&parameterGroup);
        }

        void process(shared_ptr<ofxOscMessage> oscMsg){

        }

    private:
        unsigned int nextId;
        std::map<int, shared_ptr<ResourceType>> instances;
    };
}
