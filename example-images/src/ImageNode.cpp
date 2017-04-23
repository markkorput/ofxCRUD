#include "ImageNode.h"

void ImageNode::draw(){
    image.draw(position.x, position.y);
}

void ImageNode::setFile(const string& filePath){
    this->filePath = filePath;
    image.load(filePath);
}
