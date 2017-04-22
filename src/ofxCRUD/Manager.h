#pragma once

#include "ofMain.h"
#include "ResourceDefinition.h"

namespace ofxCRUD {

    class Manager {

    public:

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
