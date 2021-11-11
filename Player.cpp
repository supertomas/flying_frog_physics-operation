#include"Player.hpp"

using namespace::std;

Item::Item(Vec2 _pos, int32 _r) :
	pos(_pos),
	r(_r)
{

}

Vec2 LoadSize(const FilePath& tomlPath)
{
	const TOMLReader toml(tomlPath);

	Vec2 size = { 0,0 };
	if (!toml)
	{
		return size;
	}

	size = Vec2(toml[U"Point.size.x"].get<int32>(), toml[U"Point.size.y"].get<int32>());

	return size;
}

Array<Vec2> Loaderror(const FilePath& tomlPath)
{
	const TOMLReader toml(tomlPath);

	Array<Vec2> errors;

	if (!toml)
	{
		return errors;
	}

	for (const auto& object : toml[U"errors"].tableArrayView())
	{
		Vec2 error;
		error = Vec2(object[U"size.x"].get<int32>(), object[U"size.y"].get<int32>());
		errors << error;
	}

	return errors;
}
Circle Item::getBody() const
{
	return Circle(pos, r);
}

void Item::draw() const
{
	auto tex = TextureAsset(U"item");
	tex.drawAt(pos.x, pos.y);
}
Player::Player(Vec2 _pos, Vec2 _size, Vec2 _speed, Array<Vec2> _errors, int32 actionsize) :
	pos(_pos),
	size(_size),
	speed(_speed),
	errors(_errors),
	action(actionsize)
{

}

void Player::checkGround(int groundPos)
{
	m_isGround = false;
	if (pos.y >= groundPos)
	{
		m_isGround = true;
		isTop = false;
		Fall_timer.reset();
		v_x = 0.0;
		v_y = 0.0;
	}
}
Quad Player::getBody() const
{
	if (action == 0 || action == 1)
	{
		Rect rect(pos.x + errors[0].x, pos.y + errors[0].y, 50, 50);
		return rect.rotated(0_deg);
	}
	else if (action == 2 || action == 3)
	{
		Rect rect(pos.x - size.x / 2 + errors[1].x, pos.y - size.y + errors[1].y, size.x, size.y);
		Quad quad;
		if (action == 2)
		{
			quad = rect.rotated(45_deg);
		}
		else if (action == 3)
		{
			quad = rect.rotated(-45_deg);
		}
		return quad;
	}

	else if (action == 4 || action == 5)
	{
		Rect rect(pos.x - size.x / 2 + errors[2].x, pos.y - size.y + errors[2].y, size.x, size.y);
		Quad quad = rect.rotated(90_deg);
		return quad;
	}

	else if (action == 6)
	{
		Rect rect(pos.x - size.x / 2 + errors[3].x, pos.y - size.y + errors[3].y, size.x, size.y);
		Quad quad = rect.rotated(90_deg);
		return quad;
	}
}

Vec2 Player::getRandom()
{
	return { Random(-1.0, 1.0) ,Random(-0.707,1.0) };
}

void Player::update()
{
	checkGround(500);
	if (action != 6)
	{
		if (const auto joy = JoyConR(0))
		{
			if (joy.buttonLR.down())
			{
				AudioAsset(U"jump").play();
				saveDirection = false;
				v_x = speed.x;
				v_y = speed.y;
				m_isGround = false;
				isTop = false;
			}
		}
		if (const auto joy = JoyConL(0))
		{
			if (joy.buttonLR.down())
			{
				AudioAsset(U"jump").play();
				saveDirection = true;
				v_x = -speed.x;
				v_y = speed.y;
				m_isGround = false;
				isTop = false;
			}
		}
		if (MouseR.down())
		{
			AudioAsset(U"jump").play();
			saveDirection = false;
			v_x = speed.x;
			v_y = speed.y;
			m_isGround = false;
			isTop = false;
		}
		else if (MouseL.down())
		{
			AudioAsset(U"jump").play();
			saveDirection = true;
			v_x = -speed.x;
			v_y = speed.y;
			m_isGround = false;
			isTop = false;
		}
	}

	if (m_isGround)
	{
		saveDirection ? action = 0 : action = 1;
	}

	else if (!m_isGround)
	{
		//x座標の更新
		if (!Right_reflection && !Left_reflection)
		{
			pos.x += v_x;
		}
		else if (Right_reflection)
		{
			pos.x += (-25);
			Right_reflection = false;
		}

		else if (Left_reflection)
		{
			pos.x += (25);
			Left_reflection = false;
		}
		if (v_y > 0 && v_x != 0)
		{
			isTop = true;
			v_x = v_x / 3.0;
			if (!Fall_timer.isRunning())
			{
				Fall_timer.restart();
			}
		}
		if (pos.y + g + v_y > 500)
		{
			pos.y = 500;
		}

		//y座標更新
		else if(!timer.isRunning())
		{
			v_y += (g - save_v_y * k);
			save_v_y = v_y;
			pos.y += v_y;
		}

		if (v_x > 0)
		{
			action = 3;
		}

		else if (v_x < 0)
		{
			action = 2;
		}
		if (v_x == 0 && saveDirection)
		{
			action = 4;
		}
		else if (v_x == 0 && !saveDirection)
		{
			action = 5;
		}
		else if (Fall_timer.ms() > 500 && saveDirection)
		{
			action = 4;
			v_x = 0;
			Fall_timer.reset();
		}
		else if (Fall_timer.ms() > 500 && !saveDirection)
		{
			action = 5;
			v_x = 0;
			Fall_timer.reset();
		}
	}



	if (timer.isRunning() && timer.ms() <= 1000)
	{
		action = 6;
		v_x = 0;
		save_v_y = 0.0;
		pos.y -= (Abs(v_y) + 20);
	}

	else
	{
		timer.reset();
	}
	if (pos.x > Scene::Width() - 50)
	{
		v_x = -v_x;
		Right_reflection = true;
		saveDirection = true;
	}

	else if (pos.x < 40)
	{
		v_x = -v_x;
		Left_reflection = true;
		saveDirection = false;
	}
}

void Player::draw() const
{
	//getBody().draw();
	if (action == 6)
	{
		auto tex = TextureAsset(U"texture3");
		tex.drawAt(pos.x, pos.y - tex.size().y / 2);
	}
	else if (action % 2 == 0)
	{
		const String name = U"texture{}"_fmt(action / 2);
		auto tex = TextureAsset(name);
		tex.drawAt(pos.x, pos.y - tex.size().y / 2);
	}
	else if (action % 2 == 1)
	{
		const String name = U"texture{}"_fmt(action / 2);
		auto tex = TextureAsset(name);
		tex.mirrored().drawAt(pos.x, pos.y - tex.size().y / 2);
	}

}