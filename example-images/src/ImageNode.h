#pragma once

#include "ofImage.h"
#include "ofVec3f.h"

class ImageNode {
    public:

        void draw();

        void setFile(const string& filePath);
        void setPosition(const ofVec3f& newPos){ position = newPos; }

    private: // CRUD attributes
        string filePath;
        ofVec3f position;

    private: // attributes
        ofImage image;
};
