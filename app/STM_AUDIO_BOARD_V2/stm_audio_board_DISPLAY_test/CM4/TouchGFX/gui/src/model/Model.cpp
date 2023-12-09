#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0), slider_value_1_u (50)
{

}

void Model::tick()
{

	modelListener->setSlider1Value(slider_value_1_u);
}
