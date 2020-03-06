#include "Appearance.h"

Appearance::Appearance(const Appearance& app)
{
	this->filepath = app.filepath;
	this->wireframe = app.wireframe;
	this->model = app.model;
	this->grayscale = app.grayscale;
	this->normalMap = app.normalMap;
	this->specularMap = app.specularMap;
	this->renderMode = app.renderMode;
}