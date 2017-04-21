#pragma once

#include "ofMain.h"

namespace ofxCRUD {

    class BaseResourceDefinition {
    protected:
        string resourceType;
    };


    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

    public:
        void setResourceType(string newResourceType){
            resourceType= newResourceType;
        }
    };

    class Manager {

    public:

        template<typename T>
        void defineResource(std::function<void (ResourceDefinition<T>&)>  func);
        //shared_ptr<ResourceDefinition<T>>
    };


    template<typename T>
    void Manager::defineResource(std::function<void (ResourceDefinition<T>&)>  func){
        ofLog() << "OK!";
    }
}
