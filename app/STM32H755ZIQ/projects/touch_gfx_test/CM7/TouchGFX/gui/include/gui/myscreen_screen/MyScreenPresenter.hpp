#ifndef MYSCREENPRESENTER_HPP
#define MYSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MyScreenView;

class MyScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MyScreenPresenter(MyScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~MyScreenPresenter() {}

private:
    MyScreenPresenter();

    MyScreenView& view;
};

#endif // MYSCREENPRESENTER_HPP
