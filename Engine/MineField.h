#pragma once

#include "Vei2.h"
#include "Graphics.h"
#include "SpriteCodex.h"

class MineField
{
public:
	MineField(int numBombs);
public:
	enum class TileState
	{
		Hiden,
		Opened,
		Flaged
	};
private:
	class Tile
	{
	public:
		void SetBomb(bool seeding);		
		void ChangeState(TileState in_state);
		void BombSeeding();
		bool HasBomb();
		void Draw(const Vei2& pos, Graphics& gfx);
	private:
		bool hasBomb;
		TileState state;
	};
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
	void SeedBombs(int nBombs);
	void Draw(Graphics& gfx);
};