#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

namespace ofxCRUD {

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
        ofParameter<PropType>& addProperty(const string& name){
            auto param = new ofParameter<PropType>();
            param->setName(name);
            parameterGroup.add(*param);
            return *param;
        }

        shared_ptr<void> createInstance(){
            return nullptr;
        }

        virtual shared_ptr<void> find(unsigned int id) = 0;
        virtual shared_ptr<ofParameterGroup> getInstanceParameters(int id) = 0;
    };


    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

        class InstanceParameterGroup : public ofParameterGroup {
        public:
            void setup(shared_ptr<ResourceType> instance, ofParameterGroup& sourceParams){
                this->instance = instance;
                this->sourceParams = &sourceParams;
            }

        private:
            shared_ptr<ResourceType> instance;
            ofParameterGroup* sourceParams;
        };

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

        template<typename PropType>
        ofParameter<PropType>& addProperty(const string& name,
            std::function<const PropType& (ResourceType&)> getterFunc = nullptr,
            std::function<void (ResourceType&, PropType& value)> setterFunc = nullptr){
            return BaseResourceDefinition::addProperty<PropType>(name);
        }

        virtual shared_ptr<ofParameterGroup> getInstanceParameters(int id){
            auto instanceParams = make_shared<InstanceParameterGroup>();
            return instanceParams;
        }

        //
        // shared_ptr<Actuator> getActuator(shared_ptr<ResourceType>){
        //     auto actuatorRef = make_shared<Actuator>();
        //     actuatorRef->setParameters(&parameterGroup);
        // }

    private:
        unsigned int nextId;
        std::map<int, shared_ptr<ResourceType>> instances;
    };
}
