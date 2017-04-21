#pragma once

#include "ofMain.h"

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

        const ofParameterGroup& getParameters(){
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
    };


    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

    public:
        void setResourceType(const string& newResourceType){
            resourceType = newResourceType;
        }

        shared_ptr<ResourceType> createInstance(){
            return make_shared<ResourceType>();
        }
    };

    class Manager {

    public:

        template<typename T>
        shared_ptr<ResourceDefinition<T>> defineResource(std::function<void (ResourceDefinition<T>&)>  func);

        const std::vector<shared_ptr<BaseResourceDefinition>>& getDefinitions() const {
            return definedResourceDefinitions;
        }

    private:
        std::vector<shared_ptr<BaseResourceDefinition>> definedResourceDefinitions;
    };


    template<typename T>
    shared_ptr<ResourceDefinition<T>> Manager::defineResource(std::function<void (ResourceDefinition<T>&)>  func){
        auto resDefRef = make_shared<ResourceDefinition<T>>();
        func(*resDefRef.get());
        definedResourceDefinitions.push_back(resDefRef);
        return resDefRef;
    }
}
