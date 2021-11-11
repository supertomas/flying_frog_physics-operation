#pragma once
#include"Scene.hpp"
#include"enemy.hpp"
#include"Player.hpp"

class Tree
{
public:
	Tree(int32 _posx, String _texture) :
		posx(_posx),
		texture(_texture)
	{

	}
	Rect getRegion(int32 links) const
	{
		return Rect(posx, Window::ClientHeight() * (-links) + 500, 80, Window::ClientHeight());
	}

	void draw() const
	{
		for (size_t i = 1; i <= 150; i++)
		{
			getRegion(i)(TextureAsset(texture)).draw();
		}
	}
private:
	int32 posx;
	String texture;

};

class Game : public MyApp::Scene
{

private:


public:
	Array<Enemy> enemies;
	Array<Tree> tree;
	Player* player;
	int32 m_score = 0;
	int32 enemies_Frequency = 1;
	int32 b = 1;
	bool dead = false;
	std::vector<double> initialEnemySpawnTime = { 2.0 ,1.7,1.6,1.4,1.3,1.2,1.1,1.0,0.9,0.8 };
	// �G�̔����Ԋu�i�b�j
	double enemySpawnTime = initialEnemySpawnTime[0];
	// �G�̔����Ԋu�^�C�}�[
	double enemySpawnTimer = 0.0;
	Array<Item> items;
	// �A�C�e���̔����Ԋu�i�b�j
	double_t ItemSpawnTime = 4.0;
	// �A�C�e���̔����Ԋu�^�C�}�[
	double_t ItemSpawnTimer = 0.0;
	Rect getRegion() const
	{
		return Rect(0, 0, Window::ClientWidth(), 600);
	}


	Game(const InitData& init)
		: IScene(init)
	{
		player = new Player({ 400,400 }, LoadSize(getData().tomlPath), { 14.0, -20.0 }, Loaderror(getData().tomlPath), 3);
		enemies.push_back(Enemy({ 50,0 }, { 120,50 }, U"crow", EnemyMoveType::Shindo));
		tree.push_back(Tree(250, U"tree2"));
		tree.push_back(Tree(-50, U"tree"));
		tree.push_back(Tree(450, U"tree2"));
		tree.push_back(Tree(Window::ClientWidth() - 50, U"tree"));
	}


	void update() override
	{
		b = -1;
		Graphics2D::SetCameraTransform(Mat3x2::Translate(0, -player->pos.y + Window::ClientSize().y / 1.5));
		const double deltaTime = Scene::DeltaTime();
		enemySpawnTimer += deltaTime;
		ItemSpawnTimer += deltaTime;
		player->update();

		while (ItemSpawnTimer > ItemSpawnTime)
		{
			if (player->m_isGround && items.size() < 2)
			{
				ItemSpawnTimer -= ItemSpawnTime;
				ItemSpawnTime = Max(ItemSpawnTime * 0.95, 0.8);
				items.push_back(Item({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, 35));
			}
			else if (!player->m_isGround && (items.size() < 6))
			{
				ItemSpawnTimer -= ItemSpawnTime;
				ItemSpawnTime = Max(ItemSpawnTime * 0.95, 0.8);
				items.push_back(Item({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, 35));
			}
			else
			{
				ItemSpawnTimer = 0.0;
			}
		}
		while (enemySpawnTimer > enemySpawnTime)
		{
			if (player->m_isGround && enemies.size() < 2)
			{
				b = Random(0, 1);
				enemySpawnTimer -= enemySpawnTime;
				enemySpawnTime = Max(enemySpawnTime * 0.95, 0.3);
				if (b == 0)
				{
					enemies.push_back(Enemy({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, { 120,50 }, U"crow", EnemyMoveType::Shindo));
				}
				else if (b == 1)
				{
					enemies.push_back(Enemy({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, { 50,50 }, U"toge", EnemyMoveType::Stop));
				}
			}
			else if (!player->m_isGround && enemies.size() < 6)
			{
				b = Random(0, 1);
				enemySpawnTimer -= enemySpawnTime;
				enemySpawnTime = Max(enemySpawnTime * 0.95, 0.3);
				if (b == 0)
				{
					enemies.push_back(Enemy({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, { 120,50 }, U"crow", EnemyMoveType::Shindo));
				}
				else if (b == 1)
				{
					enemies.push_back(Enemy({ Random(0,500),Random(player->pos.y - 1500 ,player->pos.y - 800) }, { 50,50 }, U"toge", EnemyMoveType::Stop));
				}
			}
			else
			{
				enemySpawnTimer = 0.0;
			}
		}
		if (m_score > 250 * enemies_Frequency && enemies_Frequency <= 8)
		{
			enemySpawnTime = initialEnemySpawnTime[enemies_Frequency];
			enemies_Frequency += 1;
		}

		for (auto it = items.begin(); it != items.end();)
		{
			if (it->getBody().intersects(player->getBody()))
			{
				it = items.erase(it);
				player->timer.restart();
				AudioAsset(U"hijump").play();

			}
			else
			{
				it++;
			}
		}
		Graphics2D::SetCameraTransform(Mat3x2::Identity());

		//���G��ԂœG�������������̏���
		if (player->action == 6)
		{
			for (auto& enemy : enemies)
			{
				enemy.update();
				if (enemy.type == EnemyMoveType::Stop)
				{
					if (player->getBody().intersects(enemy.TogegetBody()))
					{
						enemy.Flight_direction = player->getRandom();
						enemy.attack = true;
					}
				}
				else if (enemy.type == EnemyMoveType::Shindo)
				{
					if (player->getBody().intersects(enemy.CrowgetBody()))
					{
						enemy.Flight_direction = player->getRandom();
						enemy.attack = true;
					}
				}

			}
		}

		else
		{
			for (auto& enemy : enemies)
			{
				enemy.update();
				if (enemy.type == EnemyMoveType::Stop)
				{
					if (player->getBody().intersects(enemy.TogegetBody()))
					{
						AudioAsset(U"damage").play();
						dead = true;
						changeScene(State::GameOver);
					}
				}
				else if (enemy.type == EnemyMoveType::Shindo)
				{
					if (player->getBody().intersects(enemy.CrowgetBody()))
					{
						AudioAsset(U"damage").play();
						dead = true;
						changeScene(State::GameOver);
					}
				}
			}
		}
		for (auto& enemy : enemies)
		{
			if (enemy.attack)
			{
				enemy.pos.x += (enemy.Flight_direction.x) * 20.0;
				enemy.pos.y += (enemy.Flight_direction.y) * 20.0;
			}
		}
		//�A�C�e����6�ȏ�ɂȂ����Ƃ�7�ڂ��ǉ�����Ȃ��悤��
		//��ԍŏ��ɒǉ����ꂽ���̂���������B
		if (items.size() > 6)
		{
			if (Abs(items[0].pos.y - player->pos.y) > 1500)
			{
				items.erase(items.begin());
			}
			if (player->action == 0 || player->action == 1)
			{
				items.erase(items.begin());
			}
		}

		//�G����ʊO�ɏo�������
		for (auto it = enemies.begin(); it != enemies.end();)
		{
			if (it->pos.x < -200 || it->pos.x > Scene::Width() + 200 
				|| Abs(it->pos.y - player->pos.y) > 2500)
			{
				it = enemies.erase(it);
			}
			else
			{
				it++;
			}
		}

		m_score = Abs(player->pos.y - 506) / 20;

		getData().nowScore = m_score;
		if (m_score > getData().highScore)
		{
			getData().highScore = m_score;
		}
		for (const auto& change : getData().watcher.retrieveChanges())
		{
			if (change.first == getData().tomlPath && change.second == FileAction::Modified) // TOML �t�@�C�����X�V���ꂽ��
			{
				Print << U"reload";
				// �A�C�e�������ēǂݍ���
				player->size = LoadSize(getData().tomlPath);
				player->errors = Loaderror(getData().tomlPath);
			}
		}

	}

	void draw() const override
	{
		FontAsset(U"Score")(getData().nowScore, U"m").drawAt(Scene::Center().x + 300, 100);
		Graphics2D::SetCameraTransform(Mat3x2::Translate(0, -player->pos.y + Window::ClientSize().y / 1.2));
		getRegion()(TextureAsset(U"grass")).draw();
		for (auto& i : tree)
		{
			i.draw();
		}

		player->draw();
		for (auto& item : items)
		{
			item.draw();
		}
		for (auto& enemy : enemies)
		{
			enemy.draw();
		}
		Graphics2D::SetCameraTransform(Mat3x2::Identity());
	}


	~Game() {
		delete player;
	}

};

class GameOver : public MyApp::Scene
{
public:
	GameOver(const InitData& init)
		: IScene(init)
	{

	}
	void update() override
	{
		if (const auto joy = JoyConR(0))
		{
			if (MouseR.down() || KeyEnter.down() || joy.buttonLR.down() ||
				joy.button0.down() || joy.button1.down() || joy.button2.down() || joy.button3.down())
			{
				changeScene(State::Game);
			}
		}
		if (const auto joy = JoyConL(0))
		{
			if (MouseL.down() || KeyEnter.down() || KeyEnter.down() || joy.buttonLR.down() ||
				joy.button0.down() || joy.button1.down() || joy.button2.down() || joy.button3.down())
			{
				changeScene(State::Game);
			}
		}

		if (MouseL.down() || KeyEnter.down() || KeyEnter.down())
		{
			changeScene(State::Game);
		}
	}
	void draw() const override
	{
		FontAsset(U"Score")(U"����̃X�R�A", getData().nowScore, U"m").drawAt(Scene::Center().x, 200);
		FontAsset(U"Score")(U"�n�C�X�R�A", getData().highScore, U"m").drawAt(Scene::Center().x, 400);
	}
};