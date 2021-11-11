#pragma once
# include <Siv3D.hpp>

// �V�[���̖��O
enum class State
{
	Title,
	Game,
	GameOver
};

// �Q�[���f�[�^
struct GameData
{
	//����̃X�R�A
	int32 now = 0;
	// �n�C�X�R�A
	int32 highScore = 0;
	int32 nowScore = 0;
	FilePath tomlPath = FileSystem::FullPath(U"config.toml");
	DirectoryWatcher watcher{ FileSystem::ParentPath(tomlPath) };
};

// �V�[���Ǘ��N���X
using MyApp = SceneManager<State, GameData>;

