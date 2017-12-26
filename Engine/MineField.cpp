#include "MineField.h"
#include <random>
#include <algorithm>

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

void MineField::Tile::Draw(const Vei2& pos, bool fucked, Graphics & gfx)
{
	if (!fucked)
	{
		switch (state)
		{
		case TileState::Hiden:
			SpriteCodex::DrawTileButton(pos, gfx);
			break;
		case TileState::Opened:
			SpriteCodex::DrawTileNumber(pos, neighboursCount, gfx);
			if (HasBomb())
			{
				SpriteCodex::DrawTileBomb(pos, gfx);
			}
			break;
		case TileState::Flaged:
			SpriteCodex::DrawTileButton(pos, gfx);
			SpriteCodex::DrawTileFlag(pos, gfx);
			break;
		}
	}
	else
	{
		switch (state)
		{
		case TileState::Hiden:
			if (HasBomb())
			{
				SpriteCodex::DrawTileBomb(pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(pos, gfx);
			}
			break;
		case TileState::Opened:
			if (HasBomb())
			{
				SpriteCodex::DrawTileBombRed(pos, gfx);				
			}
			else
			{
				SpriteCodex::DrawTileNumber(pos, neighboursCount, gfx);
			}			
			break;
		case TileState::Flaged:
			if (!HasBomb())
			{
				SpriteCodex::DrawTileButton(pos, gfx);
				SpriteCodex::DrawTileFlag(pos, gfx);
				SpriteCodex::DrawTileCross(pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(pos, gfx);
				SpriteCodex::DrawTileFlag(pos, gfx);
			}			
			break;
		}
	}	
}

MineField::TileState MineField::Tile::GetState() const
{
	return state;
}

bool MineField::Tile::OpenTile()
{
	if (state == TileState::Hiden)
	{		
		state = TileState::Opened;		
		
		return HasBomb();		
	}

	return false;
}

bool MineField::Tile::IsRevealed()
{
	return state==TileState::Opened;
}

void MineField::Tile::FlagedTile()
{
	if (state != TileState::Opened)
	{
		if (state == TileState::Flaged)
		{
			state = TileState::Hiden;
		}
		else
		{
			state = TileState::Flaged;
		}
	}
}

void MineField::Tile::SetNeighboursCount(int count)
{
	neighboursCount = count;
}



int MineField::GetNeighborBombs(Vei2& pos)
{
	const int xStart = std::max(0, pos.x - 1);
	const int yStart = std::max(0, pos.y - 1);
	const int xEnd = std::min(width - 1, pos.x + 1);
	const int yEnd = std::min(height - 1, pos.y + 1);

	int nCount = 0;

	for (Vei2 tile{ xStart, yStart }; tile.y <= yEnd; tile.y++)
	{
		for (tile.x = xStart; tile.x <= xEnd; tile.x++)
		{
			if (GetTile(tile).HasBomb())
			{
				nCount++;
			}
		}
	}

	return nCount;
}

bool MineField::IsFucked() const
{
	return isFucked;
}

MineField::MineField(int numBombs, Graphics& gfx)
	:
	startPoint(gfx.ScreenWidth/2 - width*SpriteCodex::tileSize/2, gfx.ScreenHeight/2 - height*SpriteCodex::tileSize / 2)
{
	
	for (int i = 0; i < width * height; ++i)
	{
		field[i].SetBomb(false);
		field[i].ChangeState(TileState::Hiden);
	}
	SeedBombs(numBombs);
	bombsCount = numBombs;
}

MineField::Tile & MineField::GetTile(int index)
{
	return field[index];
}

MineField::Tile & MineField::GetTile(Vei2 & pos)
{
	return field[pos.y*width + pos.x];
}

void MineField::ChangeTileState(TileState in_state, int index)
{
	field[index].ChangeState(in_state);
}

void MineField::OnRevealClick(Vei2 & screenPos)
{
	Vei2 pos{ screenPos.x - startPoint.x, screenPos.y - startPoint.y };

	if (!isFucked && !isWin)
	{
		Vei2 gridPos{ (pos.x) / SpriteCodex::tileSize  , (pos.y) / SpriteCodex::tileSize };
		RevealTile(gridPos);
		
	}
	
}

void MineField::RevealTile(Vei2 & gridPos)
{
	Tile& tile = GetTile(gridPos);

	if (tile.OpenTile())
	{
		isFucked = true;
	}
	else if(HasNoNeighbours(gridPos))
	{
		const int xStart = std::max(0, gridPos.x - 1);
		const int yStart = std::max(0, gridPos.y - 1);
		const int xEnd = std::min(width - 1, gridPos.x + 1);
		const int yEnd = std::min(height - 1, gridPos.y + 1);			

		for (Vei2 tile{ xStart, yStart }; tile.y <= yEnd; tile.y++)
		{
			for (tile.x = xStart; tile.x <= xEnd; tile.x++)
			{	
				if (!GetTile(tile).IsRevealed())
				{
					ChekWin();
					hiddenTiles--;
					RevealTile(tile);
				}				
			}
		}
	}
}

void MineField::OnFlagClick(Vei2 & screenPos)
{
	screenPos -= startPoint;

	if (!isFucked && !isWin)
	{
		Vei2 gridPos{ screenPos.x / SpriteCodex::tileSize, screenPos.y / SpriteCodex::tileSize };

		Tile& tile = GetTile(gridPos);

		tile.FlagedTile();
	}

}

Vei2 MineField::GetSize() const
{
	return Vei2(width, height);
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

void MineField::ChekWin()
{
	if (hiddenTiles == bombsCount)
	{
		isWin = true;
	}

}

void MineField::Draw(Graphics & gfx)
{	
	gfx.DrawRect(startPoint.x, startPoint.y, startPoint.x + width * SpriteCodex::tileSize, startPoint.y + height * SpriteCodex::tileSize, SpriteCodex::baseColor);
	
	for (Vei2 pos{ 0, 0 }; pos.y < height; ++pos.y)
	{
		for (pos.x = 0; pos.x < width; ++pos.x)
		{
			const Vei2 screenPos{ startPoint.x + pos.x * SpriteCodex::tileSize, startPoint.y + pos.y * SpriteCodex::tileSize };
			const int i = pos.y * width + pos.x;
			field[i].Draw(screenPos, isFucked, gfx);
		}
	}
}

bool MineField::HasNoNeighbours(Vei2& pos)
{
	const int xStart = std::max(0, pos.x - 1);
	const int yStart = std::max(0, pos.y - 1);
	const int xEnd = std::min(width - 1, pos.x + 1);
	const int yEnd = std::min(height - 1, pos.y + 1);
	
	for (Vei2 tile{ xStart, yStart }; tile.y <= yEnd; tile.y++)
	{
		for (tile.x = xStart; tile.x <= xEnd; tile.x++)
		{			
			if (GetTile(tile).HasBomb())
			{
				return false;
			}
		}
	}
	return true;
}

Vei2 MineField::ToGridPos(Vei2 & screenPos)
{
	return Vei2(screenPos.x / SpriteCodex::tileSize, screenPos.y / SpriteCodex::tileSize);
}
