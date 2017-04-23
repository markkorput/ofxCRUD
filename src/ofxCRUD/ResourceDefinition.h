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

            typedef std::function<const string& (ResourceType&)> PROP_GET_FUNC;
            typedef std::function<void (ResourceType&, const string& value)> PROP_SET_FUNC;

            class PropertyDefinition : public BasePropertyDefinition {
                public:
                    void setup(const string& name, PROP_GET_FUNC getter, PROP_SET_FUNC setter){
                        setName(name);
                        getterFunc = getter;
                        setterFunc = setter;
                    }

                    virtual void set(shared_ptr<void> subject, const string& value){

                    }

                    virtual const string& get(shared_ptr<void>){
                        string a = "tmp";
                        return a;
                    }

                private:
                    PROP_GET_FUNC getterFunc;
                    PROP_SET_FUNC setterFunc;
            };

        public:

            ResourceDefinition() : nextId(1){
            }

            void setResourceType(const string& newResourceType){
                resourceType = newResourceType;
            }

            void addProperty(const string& name,
                PROP_GET_FUNC getterFunc = nullptr,
                PROP_SET_FUNC setterFunc = nullptr){
                auto propDef = make_shared<PropertyDefinition>();
                propDef->setup(name, getterFunc, setterFunc);
                propDefs[name] = propDef;
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
            std::map<string, shared_ptr<PropertyDefinition>> propDefs;
            std::map<int, shared_ptr<ResourceType>> instances;
    };
}
