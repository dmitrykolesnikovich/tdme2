#include <tdme/engine/scene/SceneLight.h>

#include <tdme/tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using tdme::engine::model::Color4;
using tdme::engine::scene::SceneLight;
using tdme::math::Vector3;
using tdme::math::Vector4;

SceneLight::SceneLight(int id)
{
	this->id = id;
	enabled = false;
	ambient.set(0.0f, 0.0f, 0.0f, 1.0f);
	diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	position.set(0.0f, 0.0f, 0.0f, 0.0f);
	spotTo.set(0.0f, 0.0f, -1.0f);
	spotDirection.set(0.0f, 0.0f, -1.0f);
	spotExponent = 0.0f;
	spotCutOff = 180.0f;
	constantAttenuation = 1.0f;
	linearAttenuation = 0.0f;
	quadraticAttenuation = 0.0f;
}
