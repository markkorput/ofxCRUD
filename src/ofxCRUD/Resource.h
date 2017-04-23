#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

const string NOTHING = "";

namespace ofxCRUD {

    class BaseResource {
        protected:
            string name;

        public:
            const string& getName(){
                return name;
            }

            void setName(const string& newName){
                name = newName;
            }

            // virtual void addProperty(const string& name) = 0;
            virtual shared_ptr<void> createInstance() = 0;
            virtual shared_ptr<void> createInstance(unsigned int id) = 0;
            virtual unsigned int getInstanceCount() = 0;
            virtual shared_ptr<void> find(unsigned int id) = 0;
            virtual bool update(unsigned int id, const string& property, const string& value) = 0;
            virtual const string& read(unsigned int id, const string& property) = 0;
            virtual void deleteInstance(unsigned int id) = 0;
            virtual void deleteInstance(shared_ptr<void> instance) = 0;
            virtual unsigned int getIdFor(shared_ptr<void> instance) = 0;
    };

    template<typename ResourceType>
    class Resource : public BaseResource {

            typedef std::function<const string& (ResourceType&)> PROP_GET_FUNC;
            typedef std::function<void (ResourceType&, const string& value)> PROP_SET_FUNC;

            class PropertyDefinition {
                public:
                    void setup(PROP_GET_FUNC getter, PROP_SET_FUNC setter){
                        getterFunc = getter;
                        setterFunc = setter;
                    }

                    void set(shared_ptr<void> subject, const string& value){
                        if(!setterFunc){
                            ofLogWarning() << "no setter func, can't set property";
                            return;
                        }

                        setterFunc(*static_pointer_cast<ResourceType>(subject).get(), value);
                    }

                    const string& get(shared_ptr<void> subject){
                        if(!setterFunc){
                            ofLogWarning() << "no setter func, can't set property";
                            return NOTHING;
                        }

                        return getterFunc(*static_pointer_cast<ResourceType>(subject).get());
                    }

                private:
                    PROP_GET_FUNC getterFunc;
                    PROP_SET_FUNC setterFunc;
            };

        public:

            Resource() : nextId(1){
            }

            void addProperty(const string& name,
                PROP_GET_FUNC getterFunc = nullptr,
                PROP_SET_FUNC setterFunc = nullptr){
                auto propDef = make_shared<PropertyDefinition>();
                propDef->setup(getterFunc, setterFunc);
                properties[name] = propDef;
            }

            virtual shared_ptr<void> createInstance(){
                return createInstance(nextId);
            }

            virtual shared_ptr<void> createInstance(unsigned int id){
                if(id < nextId){
                    ofLogWarning() << "create instance with ID (" << id << ") lower than nextId; this might overwrite an existing instance";
                }

                // create
                auto ref = make_shared<ResourceType>();
                // store
                instances[id] = ref;

                // increment auto-index cuonter
                if(id >= nextId)
                    nextId = id+1;

                // return
                return ref;
            }

            virtual shared_ptr<void> find(unsigned int id){
                auto it = instances.find(id);
                if(it == instances.end())
                    return nullptr;
                return it->second;
            }

            virtual bool update(unsigned int id, const string& property, const string& value){
                // get writer lambda, pass it the string
                auto instanceRef = find(id);
                if(!instanceRef){
                    ofLogWarning() << "could not find instance with id: " << id;
                    return false;
                }

                auto propDefRef = findPropDef(property);
                if(!propDefRef){
                    ofLogWarning() << "could not find property definition with name: " << property;
                    return false;
                }

                propDefRef->set(instanceRef, value);

                return true;
            }

            virtual const string& read(unsigned int id, const string& property){
                // get writer lambda, pass it the string
                auto instanceRef = find(id);
                if(!instanceRef){
                    ofLogWarning() << "could not find instance with id: " << id;
                    return NOTHING;
                }

                auto propDefRef = findPropDef(property);
                if(!propDefRef){
                    ofLogWarning() << "could not find property definition with name: " << property;
                    return NOTHING;
                }

                return propDefRef->get(instanceRef);
            }

            const std::map<unsigned int, shared_ptr<ResourceType>>& getInstances(){
                return instances;
            }

            virtual unsigned int getInstanceCount(){
                return instances.size();
            }

            virtual void deleteInstance(unsigned int id){
                instances.erase(id);
            }

            virtual void deleteInstance(shared_ptr<void> instance){
                deleteInstance(getIdFor(instance));
            }

            virtual unsigned int getIdFor(shared_ptr<void> instance){
                shared_ptr<ResourceType> subject = static_pointer_cast<ResourceType>(instance);
                for(auto it = instances.begin(); it != instances.end(); it++){
                    if(it->second == subject)
                        return it->first;
                }

                return 0; // zero is an invalid ID
            }

        private: // methods

            shared_ptr<PropertyDefinition> findPropDef(const string& name){
                auto it = properties.find(name);
                if(it == properties.end())
                    return nullptr;
                return it->second;
            }

        private: // attributes

            unsigned int nextId;
            std::map<string, shared_ptr<PropertyDefinition>> properties;
            std::map<unsigned int, shared_ptr<ResourceType>> instances;
    };
}
