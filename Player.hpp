#pragma once
#pragma once
#include<Siv3D.hpp>

Vec2 LoadSize(const FilePath& tomlPath);
Array<Vec2> Loaderror(const FilePath& tomlPath);
class Item
{
public:
	int32 r;
	Vec2 pos;

	Item(Vec2 _pos, int32 _r);

	Circle getBody() const;

	void draw() const;

private:
	Stopwatch vertical_jump_timer;
	String texture;
};

class Player
{
public:
	Player(Vec2 _pos, Vec2 _size, Vec2 _speed, Array<Vec2> errors, int32 texturesize);
	int32 action = 0;
	bool saveDirection = true;
	Vec2 pos, speed, size;
	Array<Vec2> errors;
	std::array<String, 3> texture;
	bool m_isGround;
	bool a;
	double_t v_x = 0.0;
	bool isTop = false;

	Stopwatch timer;

	void checkGround(int groundPos);

	Vec2 getRandom();

	void update();

	Quad getBody() const;

	void draw() const;
private:
	double_t g = 0.65;
	double_t v_y = 0.0;
	double_t save_v_y = 0.0;
	double_t k = 0.04;
	Stopwatch Fall_timer;
	bool Right_reflection = false;
	bool Left_reflection = false;
};
