#pragma once

#include "Vei2.h"
#include "Graphics.h"
#include "SpriteCodex.h"

class MineField
{
public:
	MineField(int numBombs, Graphics& gfx);
	void Draw(Graphics& gfx);	
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
		void Draw(const Vei2& pos, bool fucked, Graphics& gfx);
		TileState GetState()const;
		bool OpenTile();
		bool IsRevealed();
		void FlagedTile();
		void SetNeighboursCount(int count);
		
	private:
		bool hasBomb;
		TileState state;
		int neighboursCount = -1;
	};
public:	
	Tile& GetTile(int index);
	Tile& GetTile(Vei2& pos);
	void ChangeTileState(TileState in_state, int index);
	void OnRevealClick(Vei2& screenPos);
	void RevealTile(Vei2& gridPos);
	void OnFlagClick(Vei2& screenPos);
	Vei2 GetSize()const;
	int GetNeighborBombs(Vei2& pos);
	bool IsFucked()const;
	bool HasNoNeighbours(Vei2& pos);
	Vei2 ToGridPos(Vei2& screenPos);
	Vei2 startPoint;
private:
	static constexpr int width = 16;
	static constexpr int height = 16;
	int hiddenTiles = width * height;
	Tile field[width * height];
	void SeedBombs(int nBombs);	
	bool isFucked = false;
	bool isWin = false;
	void ChekWin();
	int bombsCount;	
};