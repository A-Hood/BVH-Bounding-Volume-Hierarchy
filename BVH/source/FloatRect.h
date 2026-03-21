#ifndef FLOATRECT_H
#define FLOATRECT_H

struct FloatRect {
	FloatRect() = default;
	FloatRect(float _left, float _top, float _width, float _height) {
		left = _left;
		top = _top;
		width = _width;
		height = _height;
	}

	float left = 0.0f;
	float top = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};

#endif