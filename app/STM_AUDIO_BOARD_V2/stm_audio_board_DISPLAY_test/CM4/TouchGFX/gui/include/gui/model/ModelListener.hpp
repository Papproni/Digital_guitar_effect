#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include "main.h"
class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }
    virtual void setSlider1Value(uint8_t val) {}
protected:
    Model* model;
    uint8_t slider_value_1_u;
};

#endif // MODELLISTENER_HPP
