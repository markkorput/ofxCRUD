#pragma once

#include "ofMain.h"

namespace ofxCRUD {

    class ResDef {

    };


    template<typename ResourceType>
    class ResourceDefinition : public ResDef {

    };



    class Manager {

    public:

        template<typename T>
        void defineResource();
        //shared_ptr<ResourceDefinition<T>>
    };


    template<typename T>
    void Manager::defineResource(){
        ofLog() << "OK!";
    }
}
