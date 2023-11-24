#ifndef MYSCREENVIEW_HPP
#define MYSCREENVIEW_HPP

#include <gui_generated/myscreen_screen/MyScreenViewBase.hpp>
#include <gui/myscreen_screen/MyScreenPresenter.hpp>

class MyScreenView : public MyScreenViewBase
{
public:
    MyScreenView();
    virtual ~MyScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // MYSCREENVIEW_HPP
