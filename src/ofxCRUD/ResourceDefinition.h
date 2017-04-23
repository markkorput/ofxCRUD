#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

namespace ofxCRUD {

    class BasePropertyDefinition {
        public:
            BasePropertyDefinition(){
            }

            void setName(const string& newName){
                name = newName;
            }

            virtual void set(shared_ptr<void> subject, const string& value) = 0;
            virtual const string& get(shared_ptr<void>) = 0;

        private:
            string name;
    };

    class BaseResourceDefinition {
        protected:
            string resourceType;

        public:
            const string& getResourceType(){
                return resourceType;
            }

            // virtual void addProperty(const string& name) = 0;
            virtual shared_ptr<void> createInstance() = 0;
            virtual shared_ptr<void> find(unsigned int id) = 0;
            virtual bool update(int id, const string& property, const string& value) = 0;
    };

    template<typename ResourceType>
    class ResourceDefinition : public BaseResourceDefinition {

            class PropertyDefinition : public BasePropertyDefinition {
                public:
                    virtual void set(shared_ptr<void> subject, const string& value){

                    }

                    virtual const string& get(shared_ptr<void>){
                        string a = "tmp";
                        return a;
                    }

                private:
                    std::function<const string& (ResourceType&)> getterFunc;
                    std::function<void (ResourceType&, const string& value)> setterFunc;
            };

        public:

            ResourceDefinition() : nextId(1){
            }

            void setResourceType(const string& newResourceType){
                resourceType = newResourceType;
            }

            void addProperty(const string& name,
                std::function<const string& (ResourceType&)> getterFunc = nullptr,
                std::function<void (ResourceType&, const string& value)> setterFunc = nullptr){
                // return BaseResourceDefinition::addProperty<PropType>(name);
            }

            virtual shared_ptr<void> createInstance(){
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

            virtual bool update(int id, const string& property, const string& value){
                // get writer lambda, pass it the string
            }

        private:
            unsigned int nextId;
            std::map<int, shared_ptr<ResourceType>> instances;
    };
}
