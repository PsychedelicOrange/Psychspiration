#pragma once
#include <bullet/LinearMath/btIDebugDraw.h>
#include <Line.h>
#include <iostream>
class DebugDrawPhysics : public btIDebugDraw
{
	mat4 mvp;
	int m_debugMode;
	Line line;
public:
	DebugDrawPhysics() : m_debugMode(1) {}
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		//i++;
		line.insertLine(vec3(from.x(), from.y(), from.z()), vec3(to.x(), to.y(), to.z()));
		//line.drawLines();
	}
	void reportErrorWarning(const char* warningString)override
	{
		std::cout << "ERROR: " << warningString;
	}

	void draw3dText(const btVector3& location, const char* textString)
	{
	}

	void setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	int getDebugMode() const
	{
		return m_debugMode;
	}
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) 
	{

	}
	void flushLines() override
	{
		line.drawLines();
		line.clearLines();
	}
	void setMVP(mat4 veiw , mat4 projection)
	{
		line.setMVP(veiw,projection);
	}
};