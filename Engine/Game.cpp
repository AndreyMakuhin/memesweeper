/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <random>

//это моя ветка теперь...тест
//запаблишил, еще один тест

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	mineField(50)
{
	/*
	//just for test
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDistr(0, 19);
	std::uniform_int_distribution<int> yDistr(0, 15);

	for (int i = 0; i < 220; ++i)
	{		
		int index;

		do
		{
		Vei2 cell{ xDistr(rng), yDistr(rng) };
		index = cell.y * 20 + cell.x;

		mineField.ChangeTileState(MineField::TileState::Opened, index);

		} while (mineField.GetTile(index).GetState() != MineField::TileState::Opened);
	}
	//just for test
	*/
	for (Vei2 pos{ 0,0 }; pos.y < mineField.GetSize().y; ++pos.y)
	{
		for (pos.x = 0; pos.x < mineField.GetSize().x; ++pos.x)
		{
			const Vei2 screenPos{ pos.x * SpriteCodex::tileSize, pos.y * SpriteCodex::tileSize };
			mineField.GetTile(pos).SetNeighboursCount( mineField.GetNeighborBombs(pos));
		}
	}
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (wnd.mouse.GetPosX()>=0 && wnd.mouse.GetPosX() < mineField.GetSize().x * SpriteCodex::tileSize &&
		wnd.mouse.GetPosY() >= 0 && wnd.mouse.GetPosY() < mineField.GetSize().y * SpriteCodex::tileSize)
	{
		while (!wnd.mouse.IsEmpty())
		{
			const Mouse::Event e = wnd.mouse.Read();
			const Vei2 gridPos{e.GetPosX()/SpriteCodex::tileSize, e.GetPosY()/SpriteCodex::tileSize};

			if (e.GetType() == Mouse::Event::Type::LPress)
			{
				//mineField.OnMouseClick(Vei2{ e.GetPosX(), e.GetPosY() });
				mineField.GetTile(gridPos.y * mineField.GetSize().x + gridPos.x).OnOpenTile();
			}
			else if (e.GetType() == Mouse::Event::Type::RPress)
			{
				mineField.GetTile(gridPos.y * mineField.GetSize().x + gridPos.x).OnFlagedTile();
			}
		}		
	}	
}

void Game::ComposeFrame()
{
	mineField.Draw(gfx);
}
