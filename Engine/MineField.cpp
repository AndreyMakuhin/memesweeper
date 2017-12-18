#include "MineField.h"
#include <random>

void MineField::Tile::SetBomb(bool seeding)
{
	hasBomb = seeding;
}

void MineField::Tile::ChangeState(TileState in_state)
{
	state = in_state;
}

void MineField::Tile::BombSeeding()
{
	hasBomb = true;
}

bool MineField::Tile::HasBomb()
{
	return hasBomb;
}

void MineField::Tile::Draw(const Vei2& pos, Graphics & gfx)
{
	switch (state)
	{
	case TileState::Hiden:
		SpriteCodex::DrawTileButton(pos, gfx);
		break;
	case TileState::Opened:
		SpriteCodex::DrawTile0(pos, gfx);
		if (HasBomb())
		{
			SpriteCodex::DrawTileBomb(pos, gfx);
		}
	case TileState::Flaged:
		SpriteCodex::DrawTileButton(pos, gfx);
		SpriteCodex::DrawTileFlag(pos, gfx);
	}
	
}

MineField::TileState MineField::Tile::GetState() const
{
	return state;
}

MineField::MineField(int numBombs)
{
	for (int i = 0; i < width * height; ++i)
	{
		field[i].SetBomb(false);
		field[i].ChangeState(TileState::Hiden);
	}
	SeedBombs(numBombs);
}

MineField::Tile & MineField::GetTile(int index)
{
	return field[index];
}

void MineField::ChangeTileState(TileState in_state, int index)
{
	field[index].ChangeState(in_state);
}

void MineField::SeedBombs(int nBombs)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDistr(0, width - 1);
	std::uniform_int_distribution<int> yDistr(0, width - 1);

	for (int i = nBombs; i > 0; --i)
	{	
		int a;
		do
		{
			a = xDistr(rng) * width + yDistr(rng);
		} while (field[a].HasBomb());

		field[a].BombSeeding();
	}
}

void MineField::Draw(Graphics & gfx)
{
	gfx.DrawRect(0, 0, width * SpriteCodex::tileSize, height * SpriteCodex::tileSize, SpriteCodex::baseColor);
	
	for (Vei2 pos{ 0,0 }; pos.y < height; ++pos.y)
	{
		for (pos.x = 0; pos.x < width; ++pos.x)
		{
			const Vei2 screenPos{ pos.x * SpriteCodex::tileSize, pos.y * SpriteCodex::tileSize };
			const int i = pos.x * width + pos.y;
			field[i].Draw(screenPos, gfx);
		}
	}
}
