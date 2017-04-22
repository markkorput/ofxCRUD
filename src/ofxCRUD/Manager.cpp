#include "Manager.h"

using namespace ofxCRUD;


// template<typename T>
// shared_ptr<ResourceDefinition<T>> Manager::defineResource(std::function<void (ResourceDefinition<T>&)> definitionRef){
//
// }

// manager.defineResource<ImageNode>([](ResourceDefinition<ImageNode>& def){
//     def->setResourceTypeName("ImageNode");
//     // def->addInstantiator([](){ return make_shared<ImageNode>(); }); // optional, default to this
//     def->addProperty<string>("file",                                    // property name
//         [](ImageNode &node){ return node.getFileName(); },              // getter method that extracts a value from the instance
//         [](ImageNode &node, string& value){ node.setFileName(value); });// setter method that pushes a value into the instance
//     def->addProperty<int>("status");
//     def->addProperty<ofVec3f>("position");
//     def->addProperty<ofVec3f>("scale");
// });
