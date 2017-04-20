#pragma once

#include "ofMain.h"
#include "ofParameter.h"
#include "LambdaEvent.h"

namespace ofxCRUD {

    template<typename Type>
    class Parameter : public ofParameter<Type> {

    public:
        const Type & get();

    public:
        LambdaEvent<Parameter<Type>> getEvent;
    };
}

using namespace ofxCRUD;

template<typename Type>
const Type& Parameter<Type>::get(){
    // notify getEvent listener who will have a chance to
    // change (update?) the value of this param before it is returned
    getEvent.notifyListeners(*this);
    return ofParameter<Type>::get();
}
