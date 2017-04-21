#pragma once

#include "ofMain.h"

namespace ofxCRUD {

    class BaseResourceDefinition {
    protected:
        string resourceType;
        ofParameterGroup parameterGroup;

    public:

        const string& getResourceType(){
            return resourceType;
        }

        const ofParameterGroup& getParameters(){
            return parameterGroup;
        }
    };


    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

    public:
        void setResourceType(const string& newResourceType){
            resourceType = newResourceType;
        }

        template<typename PropType>
        void addProperty(const string& name){
            auto param = new ofParameter<PropType>();
            param->setName(name);
            parameterGroup.add(*param);
        }
    };

    class Manager {

    public:

        template<typename T>
        shared_ptr<ResourceDefinition<T>> defineResource(std::function<void (ResourceDefinition<T>&)>  func);
        //shared_ptr<ResourceDefinition<T>>
    };


    template<typename T>
    shared_ptr<ResourceDefinition<T>> Manager::defineResource(std::function<void (ResourceDefinition<T>&)>  func){
        auto resDefRef = make_shared<ResourceDefinition<T>>();
        func(*resDefRef.get());
        return resDefRef;
    }
}
