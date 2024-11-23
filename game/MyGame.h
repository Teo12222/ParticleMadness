#pragma once

#include "SpriteBar.h"
#include <vector>
#include <fstream>
#include <map>

struct gridspace {
	int type;
	int i;
	int j;
	float velx;
	float vely;
	float rotation;
	bool bouncing;
	int time;
	bool visited;
};


class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSpriteOval cursor;
	gridspace** grid;
	int Width;
	int Height;

	enum BlockType {AIR=0, WOOD=4, BOUNCE=3, SAND=1, WATER=2, GREEN_WATER=5, GREEN_CHANGER=6, ENDBLOCK=8};
	BlockType goalBlock;
	CVector generatorPos;
	BlockType generatorBlock;
	float amountOfWood; float amountOfBounce;
	// Have a way to save a file for each level - Top row list of all blocks - Second row - x and y and type of generator and goal block
	BlockType selectedBlock;
	enum {FREEHAND, LINE, SQUARE} selectedShape;
	CSprite selectedShapeRect; CSprite selectedBlockRect;

	CSprite mainMenu;
	CSprite levelSelect;
	vector<CSprite*> mainButtons;
	CSprite background;
	CSprite gameMenu;
	CSprite buttonsImg;
	vector<CSprite*> gameButtons;
	CSprite endMenu;
	vector<CSprite*> endButtons;
	SpriteBar woodBar;
	SpriteBar bounceBar;
	SpriteBar goalBar;

	gridspace* previousBlock;
	vector<gridspace*> currentShape;

	enum {MAIN, PLAY, END} playState;

	CSoundPlayer drawSound;
	CSoundPlayer buttonsSound;
	CSoundPlayer winSound;
	CSoundPlayer music;

public:
	CMyGame(void);
	~CMyGame(void);

	

	vector<gridspace*> GetBetweenPoints(gridspace* start, gridspace* end);
	void LoadLevel(int level);
	void SaveLevel();
	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
