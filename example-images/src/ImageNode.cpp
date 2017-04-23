#include "ImageNode.h"

void ImageNode::registerCrud(ofxCRUD::Manager& crud){
    crud.defineResource<ImageNode>([](ResourceDefinition<ImageNode>& def){
        def.setResourceType("ImageNode");
        def.addProperty("file",
            nullptr, // no getter for now; write-only!
            [](ImageNode& node, const string& value){ node.setFile(value); });
    });
}

void ImageNode::draw(){
    image.draw(position.x, position.y);
}

void ImageNode::setFile(const string& filePath){
    this->filePath = filePath;
    image.load(filePath);
}
