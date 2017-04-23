#pragma once

#include "ofImage.h"
#include "ofVec3f.h"
#include "ofxCRUD.h"

class ImageNode {

    public: // static methods

        static void createResource(ofxCRUD::Manager& crud);

    public: // instance methods

        void draw();

        void setFile(const string& filePath);
        void setPosition(const ofVec3f& newPos){ position = newPos; }

    private: // CRUD attributes

        string filePath;
        ofVec3f position;

    private: // attributes

        ofImage image;
};
