#include <gui/mainscreen_screen/mainScreenView.hpp>

mainScreenView::mainScreenView()
{

}

void mainScreenView::setupScreen()
{
    mainScreenViewBase::setupScreen();
}

void mainScreenView::tearDownScreen()
{
    mainScreenViewBase::tearDownScreen();
}

// test if value changes on screen
void mainScreenView::set_parameter_value(uint8_t val_u,uint8_t sliderID_u){
	this->slider1.setValue(50);
	this->slider2.setValue(10);
	this->slider3.setValue(60);
	this->slider4.setValue(90);
}

