// OF & addons
#include "ofVec3f.h"
#include "ofxUnitTests.h"
// local
#include "ofxCRUD.h"

#define TEST_START(x) {ofLog()<<"CASE: "<<#x;
#define TEST_END }

using namespace ofxCRUD;

class ImageNode {
public:
    ImageNode(){ status = "uninitialized"; }
    string status;
    string filename;
    ofVec3f position, scale;

    string getStatus(){ return status; }
};

class ofApp: public ofxUnitTestsApp{

    void run(){
        TEST_START(LambdaEvent)
            ofLog() << "TODO";
        TEST_END


        TEST_START(Parameter::getEvent)
            Parameter<int> number;

            // normal ofParameter behaviour
            number.set(123);
            test_eq(number.get(), 123, "");
            test_eq(number.get(), 123, "");

            // getEvent
            number.getEvent += [](Parameter<int>& param){
                // change param value before it respond to the next .get call
                param.set(456);
            };

            test_eq(number.get(), 456, "");
        TEST_END


        TEST_START(Manager)
            // Add/Define a resouce
            ofxCRUD::Manager manager;

            test_eq(manager.getResourceDefinitions().size(), 0, "");
            auto resDefRef = manager.defineResource<ImageNode>([](ResourceDefinition<ImageNode>& def){
                def.setResourceType("ImageNode");
                // // def->addInstantiator([](){ return make_shared<ImageNode>(); }); // optional, default to this
                // def->addProperty<string>("file",                                    // property name
                //     [](ImageNode &node){ return node.getFileName(); },              // getter method that extracts a value from the instance
                //     [](ImageNode &node, string& value){ node.setFileName(value); });// setter method that pushes a value into the instance
                def.addProperty<string>("status");
                def.addProperty<ofVec3f>("position");
                def.addProperty<ofVec3f>("scale");
            });


            TEST_START(Manager::defineResource)
                test_eq(manager.getResourceDefinitions().size(), 1, "");
                test_eq(manager.getResourceDefinitions()[0]->getResourceType(), "ImageNode", "");

                // first property is an int-based ID, always added at instantiation
                test_eq(resDefRef->getParameters().getType(0), typeid(ofParameter<int>).name(), "");
                test_eq(resDefRef->getParameters().getType(1), typeid(ofParameter<string>).name(), "");
                test_eq(resDefRef->getParameters().getType(2), typeid(ofParameter<ofVec3f>).name(), "");
                test_eq(resDefRef->getParameters().getType(3), typeid(ofParameter<ofVec3f>).name(), "");
                test_eq(resDefRef->getParameters().getName(0), "id", "");
                test_eq(resDefRef->getParameters().getName(1), "status", "");
                test_eq(resDefRef->getParameters().getName(2), "position", "");
                test_eq(resDefRef->getParameters().getName(3), "scale", "");
            TEST_END

            shared_ptr<ImageNode> firstImageNodeRef = resDefRef->createInstance();
            TEST_START(CREATE)

                // "/ofxCRUD/ImageNode/create/start"
                // "/ofxCRUD/ImageNode/create/status", 2
                // "/ofxCRUD/ImageNode/create/position", 2,4,3
                // "/ofxCRUD/ImageNode/create/scale", 1,1,1
                // "/ofxCRUD/ImageNode/create/end"

                test_eq(firstImageNodeRef == nullptr, false, "");
                test_eq(firstImageNodeRef->getStatus(), "uninitialized", "");
            TEST_END

            TEST_START(UPDATE)

                // "/ofxCRUD/ImageNode/update/1/start"
                // "/ofxCRUD/ImageNode/update/1/status", 2
                // "/ofxCRUD/ImageNode/update/1/position", 2,4,3
                // "/ofxCRUD/ImageNode/update/1/scale", 1,1,1
                // "/ofxCRUD/ImageNode/update/1/end"

                auto resDefRef = manager.getResourceDefinition("ImageNode");
                // find existing instance; failure
                shared_ptr<void> voidRef = resDefRef->find(123); // 1 is default first id
                test_eq(voidRef == nullptr, true,  "");
                // find existing instance; success
                voidRef = resDefRef->find(1); // 1 is default first id
                test_eq(voidRef == nullptr, false,  "");
                test_eq(static_pointer_cast<ImageNode>(voidRef), firstImageNodeRef,  "");
                // still has original value
                test_eq(static_pointer_cast<ImageNode>(voidRef)->status, "uninitialized", "");
                // perform update


            TEST_END
        TEST_END


            //
            // auto ref = make_shared<Facade>();
            //
            // template<typename T>
            // ref->setCreate([](){
            //     return make_shared<T>();
            // })
            //
            // class ResourceDefinition {
            //     void process(OperationData data){
            //         if(action == "create")
            //             create(data)
            //         else if(...)
            //             //...
            //     }
            //
            //     void doCreate(OperationData data){
            //         // instantiate
            //         shared_ptr<void> instance = createLambda(data);
            //         setInstance(instance); // this
            //
            //         // update writes to internal paramGroup, which in turn should trigger
            //         // the write callbacks registered uing addProperty calls
            //         update(data);
            //
            //         list.push_back(instance);
            //     }
            //
            //     void doRead(OperationData data){
            //         auto ref = findById(data.getId());
            //         if(ref == nullptr){
            //             // 404 WARNING
            //             return;
            //         }
            //
            //         setInstance(ref);
            //         serializeParams();
            //     }
            //
            //     void doUpdate(OperationData data){
            //         auto ref = findById(data.getId());
            //         if(ref == nullptr){
            //             // 404 WARNING
            //             return;
            //         }
            //
            //         setInstance(ref);
            //         deserializeParams();
            //     }
            //
            //     void doDelete(OperationData data){
            //         shared_ptr<void> ref = findById(data.getId());
            //         if(ref == nullptr){
            //             // 404 WARNING
            //             return;
            //         }
            //
            //         list.remove(ref);
            //     }



            // }

            // incoming operation data (basically an ofParam set);
            // {action<string>: "create"
            // resourceName<string>: "ImageNode"
            // attributes: {
            //   position<string>: "0.0,1.0,10.0",
            //   status<string>: "1"
            // }

            // if(data.resourceName == "ImageNode"){
            //     auto opRef = make_shared<ResourceManager<ImageNode>>();
            //     opRef->processData(data);
            // } else if(data.resourceName == "TextNode"){
            //     auto opRef = make_shared<ResourceManager<ImageNode>>();
            //     opRef->processData(data);
            // }
            //
            // for(auto definitionRef : knownDefinitions){
            //     if(definitionRef.resourceName == operationData.resourceName){
            //         definitionRef.process(operationData);
            //     }
            // }
    }
};


#include "ofAppNoWindow.h"
#include "ofAppRunner.h"

int main( ){
  ofInit();
  auto window = std::make_shared<ofAppNoWindow>();
  auto app = std::make_shared<ofApp>();
  ofRunApp(window, app);
  return ofRunMainLoop();
}
