#pragma once
class MouseMovementEvent {
	double x, y;
	MouseMovementEvent(double xpos, double ypos) : x(xpos), y(ypos) {}
};