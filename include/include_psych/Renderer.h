#pragma once
#include <Shader.h>
#include <RObject.h>
#include <RenderState.h>
#include <utility>
using std::pair;
class Renderer
{
public:
	vector<pair<RenderState,RObject*>> renderList;

	void addToRenderList(RenderState renderState,vector<RObject*>& objects)
	{
		for (auto object : objects)
			renderList.push_back(pair(renderState, object));
	}
	void render()
	{
		for (auto const& renderItem : renderList)
		{
			setState(renderItem.first);
			draw();
		}
	}
	void setState(RenderState renderState)
	{

	}
	void draw()
	{

	}
};