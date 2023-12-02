#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/mainScreenViewBase.hpp>
#include <gui/mainscreen_screen/mainScreenPresenter.hpp>

class mainScreenView : public mainScreenViewBase
{
public:
    mainScreenView();
    virtual ~mainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void set_parameter_value(uint8_t val_u,uint8_t sliderID_u); // sets slider value
protected:
private:
    uint8_t param1_u = 25;
    uint8_t param2_u = 30;
    uint8_t param3_u = 50;
    uint8_t param4_u = 75;
};

#endif // MAINSCREENVIEW_HPP
