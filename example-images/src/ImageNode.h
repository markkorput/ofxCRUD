#pragma once

#include "ofImage.h"
#include "ofVec3f.h"
#include "ofxCRUD.h"

namespace Resources {
    class ValueNode {

        public: // instance methods

            void setValue(float value){ this->value = value }
            float getValue(){ return value; }

        private: // CRUD attributes

            float value;

    };
}
