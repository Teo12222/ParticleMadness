#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : cursor(400, 300, 5, CColor::White(), CColor::White(), GetTime()),
selectedShapeRect(0, 0, "greenBorder.png", CColor::Green(), GetTime()),
selectedBlockRect(0, 0, "greenBorder.png", CColor::Green(), GetTime()),
woodBar(1057, 205, 560, 50, CColor(96, 60, 17), GetTime()),
bounceBar(1057, 100, 560, 50, CColor::Red(), GetTime()),
goalBar(1824, 541, 50, 442, CColor(96, 60, 17), GetTime()),
levelSelect(-400, -400, "levelPlay.png", GetTime())
// to initialise more sprites here use a comma-separated list
{

	woodBar.init();
	bounceBar.init();
	goalBar.init(false);
	grid = nullptr;

}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

vector<gridspace*> CMyGame::GetBetweenPoints(gridspace* start, gridspace* end)
{
	vector<gridspace*> points = {};

	if (start->i == end->i && start->j == end->j) return points;
	int xDiff = start->j - end->j;
	int yDiff = start->i - end->i;
	bool xDiffIsLarger = abs(xDiff) > abs(yDiff);

	int xModifier = xDiff < 0 ? 1 : -1;
	int yModifier = yDiff < 0 ? 1 : -1;

	int longerSideLength = max(abs(xDiff), abs(yDiff));
	int shorterSideLength = min(abs(xDiff), abs(yDiff));
	float slope = (shorterSideLength == 0 || longerSideLength == 0) ? 0 : ((float)(shorterSideLength) / (longerSideLength));

	int shorterSideIncrease;
	for (int i = 1; i <= longerSideLength; i++) {
		shorterSideIncrease = round(i * slope);
		int yIncrease, xIncrease;
		if (xDiffIsLarger) {
			xIncrease = i;
			yIncrease = shorterSideIncrease;
		}
		else {
			yIncrease = i;
			xIncrease = shorterSideIncrease;
		}
		int currentY = start->i + (yIncrease * yModifier);
		int currentX = start->j + (xIncrease * xModifier);
		if (currentY >= 0 && currentY < Height && currentX >= 0 && currentX < Width);
		points.push_back(&grid[currentY][currentX]);
	}
	return points;
}

void CMyGame::OnUpdate()
{
	if (playState == MAIN) return;
	Uint32 t = GetTime();
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			grid[i][j].visited = false;
		}
	}
	for (int i = 55; i < Height; i++) {
		for (int j = 41; j < Width-41; j++) {
			if (!grid[i][j].visited) {
				grid[i][j].time++;
				if (grid[i][j].time % 2 == 0) {
					if (grid[i][j].type == SAND || grid[i][j].type == WATER || grid[i][j].type == GREEN_WATER) {
						gridspace cell = grid[i][j];
						int predictedY = i + round(grid[i][j].vely); int predictedX = j + round(grid[i][j].velx);
						if (predictedY >= 0 && predictedY < Height && predictedX >= 0 && predictedX < Width) {
							vector<gridspace*> points = GetBetweenPoints(&grid[i][j], &grid[predictedY][predictedX]);
							for (gridspace* point : points) {

								if (point->type == AIR || point->type == GREEN_CHANGER) {
									grid[cell.i][cell.j].type = 0;
									grid[cell.i][cell.j].velx = 0;
									grid[cell.i][cell.j].vely = 0;
									grid[cell.i][cell.j].bouncing = false;
									if (cell.type == WATER && grid[point->i][point->j].type == GREEN_CHANGER) {
										grid[point->i-1][point->j].type = GREEN_WATER;
										grid[point->i-1][point->j].velx = cell.velx;
										grid[point->i-1][point->j].vely = cell.vely;
										grid[point->i-1][point->j].bouncing = cell.bouncing;
										cell = grid[point->i-1][point->j];
									}
									else {
										grid[point->i][point->j].type = cell.type;
										grid[point->i][point->j].velx = cell.velx;
										grid[point->i][point->j].vely = cell.vely;
										grid[point->i][point->j].bouncing = cell.bouncing;
										cell = grid[point->i][point->j];
									}
									
									
								}
								else {
									// If a piece of sand hits a bouncing platform
									if ((grid[cell.i][cell.j].type == SAND && grid[point->i][point->j].type == BOUNCE)) {
										// It bounces either from the top or bottom of the platform, determined if the velocity is going towards the top or bottom
										float restitution = 2;
										CVector v = CVector(cell.velx, cell.vely);
										float rotation = grid[point->i][point->j].rotation;
										CVector n(sin(rotation), cos(rotation));
										if (Dot(v, n) < 0) {
											CVector newVel = restitution * Reflect(v, n);
											grid[cell.i][cell.j].velx = abs(newVel.X()) <= 4 ? newVel.X() : 4 * newVel.X() / abs(newVel.X());
											grid[cell.i][cell.j].vely = abs(newVel.Y()) <= 2 ? newVel.Y() : 2 * newVel.Y() / abs(newVel.Y());
											grid[cell.i][cell.j].bouncing = true;
										}
										n = CVector(-sin(rotation), -cos(rotation));
										if (Dot(v, n) < 0) {
											CVector newVel = restitution * Reflect(v, n);
											grid[cell.i][cell.j].velx = abs(newVel.X()) <= 4 ? newVel.X() : 4 * newVel.X() / abs(newVel.X());
											grid[cell.i][cell.j].vely = abs(newVel.Y()) <= 2 ? newVel.Y() : 2 * newVel.Y() / abs(newVel.Y());
											grid[cell.i][cell.j].bouncing = true;
										}
										/*n = CVector(cos(rotation), -sin(rotation));
										if (Dot(v, n) < 0) {
											CVector newVel = restitution * Reflect(v, n);
											grid[cell.i][cell.j].velx = newVel.X();
											grid[cell.i][cell.j].vely = newVel.Y();
											grid[cell.i][cell.j].bouncing = true;
										}
										n = CVector(-cos(rotation), sin(rotation));
										if (Dot(v, n) < 0) {
											CVector newVel = restitution * Reflect(v, n);
											grid[cell.i][cell.j].velx = newVel.X();
											grid[cell.i][cell.j].vely = newVel.Y();
											grid[cell.i][cell.j].bouncing = true;
										}*/
									}
									// If sand hits the wood, it should stop acting like it is bouncing, and go back to acting like regular sand
									if (grid[cell.i][cell.j].type == SAND && grid[point->i][point->j].type == WOOD) {
										grid[cell.i][cell.j].bouncing = false;
									}
									if (grid[point->i][point->j].type != AIR) {
										// if a blocks hits the same type of material it bounces off eahother
										if (grid[point->i][point->j].type == grid[cell.i][cell.j].type) {
											CVector vel1(grid[cell.i][cell.j].velx, grid[cell.i][cell.j].vely);
											CVector vel2(grid[point->i][point->j].velx, grid[point->i][point->j].vely);
											CVector n = Normalise(CVector(grid[point->i][point->j].j - grid[cell.i][cell.j].j, grid[point->i][point->j].i - grid[cell.i][cell.j].i));
											CVector velChange = Dot(vel1 - vel2, n) * n;

											grid[cell.i][cell.j].velx -= velChange.X();
											grid[cell.i][cell.j].vely -= velChange.Y();

											grid[point->i][point->j].velx += velChange.X();
											grid[point->i][point->j].vely += velChange.Y();
											
											// If a piece of sand was bouncing and hits another piece of sand it should send the other piece of sand bouncing as well.
											if (grid[cell.i][cell.j].type == SAND && grid[cell.i][cell.j].bouncing) {
												// If the other piece of sand was was still, it meant that it was on a wood surface, so the sand that hit it should stop bouncing
												if (!grid[point->i][point->j].bouncing && round(grid[point->i][point->j].velx) == 0 && round(grid[point->i][point->j].vely == 0))
													grid[cell.i][cell.j].bouncing = false;
												grid[point->i][point->j].bouncing = true;
											}
										}
										else {
											if (grid[cell.i][cell.j].velx != 0) {
  												grid[cell.i][cell.j].velx = -grid[cell.i][cell.j].velx;
											}
										}
									}
									// Sets cell to the new place of the block that moved, so now the changes are done to that block
									cell = grid[cell.i][cell.j];
									break;
								}
							}
							

						}
						// Sets visited to true to the position of the block after moving
						grid[cell.i][cell.j].visited = true;

						// Makes sure that the a block is moving down if there is air underneath it.
						if (grid[cell.i - 1][cell.j].type == AIR && (!grid[cell.i][cell.j].bouncing) /*|| round(grid[cell.i][cell.j].vely) == 0)*/ && (int)grid[cell.i][cell.j].vely >= 0 /*((grid[cell.i][cell.j].vely == 0 && grid[cell.i][cell.j].type == 1) || (round(grid[cell.i][cell.j].vely) == 0 && grid[cell.i][cell.j].type == 2))*/) { // typecast makes water work, but breaks bounce. // Maybe first look at how to create bouncing surface
							grid[cell.i][cell.j].vely = -1;
						}

						if (grid[cell.i - 1][cell.j].type != AIR && !grid[cell.i][cell.j].bouncing && grid[cell.i - 1][cell.j].type != BOUNCE && grid[cell.i - 1][cell.j].type != GREEN_CHANGER) {
							// Checks diagonally only if there is nothing underneath the block - We don't want to do this check if the block is bouncing, about to bounce or about to change
							if (grid[cell.i - 1][cell.j - 1].type == AIR && rand() % 100 > 2) { // change 2 to determine the chance that it moves down
								grid[cell.i][cell.j].velx -= 0.2;
							}
							else if (grid[cell.i - 1][cell.j + 1].type == AIR && rand() % 100 > 2) {
								grid[cell.i][cell.j].velx += 0.2;
							}
							// Checks horizontally if the material is a kind of water. But it moves only if the material is already moving in a particular direction.
							else if (grid[cell.i][cell.j].type == WATER || grid[cell.i][cell.j].type == GREEN_WATER) {
								if (grid[cell.i][cell.j + 1].type == AIR && ((grid[cell.i][cell.j].velx > 0.2 && grid[cell.i][cell.j].velx <= 2) || grid[cell.i][cell.j - 1].type != AIR) && rand() % 100 > 80) {
									grid[cell.i][cell.j].velx += 0.2;
								}
								else if (grid[cell.i][cell.j - 1].type == AIR && ((grid[cell.i][cell.j].velx < -0.2 && grid[cell.i][cell.j].velx >= -2) || grid[cell.i][cell.j + 1].type != AIR) && rand() % 100 > 80) {
									grid[cell.i][cell.j].velx -= 0.2;
								}
								
							}
							else {
								grid[cell.i][cell.j].velx = 0;
							}

						}
						// If there is a block underneath after moving, it means that the block just hit it or is moving sideways.
						if (grid[cell.i - 1][cell.j].type != AIR && !grid[cell.i][cell.j].bouncing && grid[cell.i - 1][cell.j].type != BOUNCE && grid[cell.i - 1][cell.j].type != GREEN_CHANGER) {
							if (grid[cell.i][cell.j].vely != 0) {
								// Force transfer
								float side;
								if (grid[cell.i][cell.j].velx > 0) {
									side = -1.f;
								}
								else if (grid[cell.i][cell.j].velx < 0) {
									side = 1.f;
								}
								else if (grid[cell.i][cell.j].velx == 0) {
									side = rand() % 2 == 0 ? 1.f : -1.f;
								}
								grid[cell.i][cell.j].velx += side * (float)grid[cell.i][cell.j].vely * 3.f / 3.f;
								grid[cell.i][cell.j].vely = 0;
							}
							else {
								//Friction
								grid[cell.i][cell.j].velx *= 0.9;
							}
						}
						else {
							// Air resistance
							if (grid[cell.i][cell.j].velx != 0 && !grid[cell.i][cell.j].bouncing) {
								grid[cell.i][cell.j].velx *= 0.55;
							}
							// Gravity 
							if (grid[cell.i][cell.j].vely >= -2)
								grid[cell.i][cell.j].vely -= 0.2;
						}

						
					}
					else if (grid[i][j].type == BOUNCE) {
						// If a bouncing platform as a sand block on top of hit that is not moving, it will make it bounce.
						if (grid[i + 1][j].type == SAND && round(grid[i + 1][j].velx) == 0 && round(grid[i + 1][j].vely) == 0 && rand() % 100 > 60) {
							CVector v = CVector(0, -2);
							float rotation = grid[i][j].rotation;
							CVector n(sin(rotation), cos(rotation));
							CVector newVel = Reflect(v, n);
							grid[i + 1][j].velx = newVel.X();
							grid[i + 1][j].vely = newVel.Y();
							grid[i + 1][j].bouncing = true;
							
						}
					}
					else if (grid[i][j].type == ENDBLOCK) {
						// Removes any block on top of it until the goal is reached
						if (goalBar.GetProgress() < 100.f) {
							// If the block of it is the same as the goal of the level, it adds one to the bar
							if (grid[i + 1][j].type == goalBlock)
								goalBar.SetProgress(goalBar.GetProgress() + 1.f);
							grid[i + 1][j].type = 0;
							grid[i + 1][j].velx = 0;
							grid[i + 1][j].vely = 0;
						}
					}
				}
			}
		} 
	}
	// Creates a generator of the specified material at a specific point
	if (grid != nullptr && rand() % 100 > 50) {
		int i = generatorPos.Y(); int j = generatorPos.X();
		grid[i][j].type = generatorBlock;
		grid[i][j].velx = (float)(-6 + rand() % 13) / 2.f;

	}
	if (goalBar.GetProgress() >= 100.f && playState != END) {
		playState = END;
		winSound.Play("win.wav");
	}
	cursor.Update(t);
}

void CMyGame::OnDraw(CGraphics* g)
{
	if (playState == MAIN) {
		mainMenu.Draw(g);
		levelSelect.Draw(g);
	}
	else {
		background.Draw(g);
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width; j++) {
				if (grid[i][j].type == SAND)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor(236, 204, 162));
				if (grid[i][j].type == WATER)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor(68, 187, 255));
				if (grid[i][j].type == GREEN_WATER)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor(68, 238, 187));
				if (grid[i][j].type == GREEN_CHANGER)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor(27, 93, 73));
				if (grid[i][j].type == BOUNCE)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor::Red());
				if (grid[i][j].type == WOOD)
					g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor(96, 60, 17));
				if (grid[i][j].type == ENDBLOCK) {
					if (goalBlock == SAND)
						g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor::Yellow());
					else if (goalBlock == WATER)
						g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor::Blue());
					else if (goalBlock == GREEN_WATER)
						g->FillRect(CRectangle(grid[i][j].j * GetWidth() / Width, grid[i][j].i * GetHeight() / Height, GetWidth() / Width + 1, GetHeight() / Height + 1), CColor::Green());
				}
			}
		}
		gameMenu.Draw(g);

		selectedBlockRect.Draw(g);
		selectedShapeRect.Draw(g);
		woodBar.Draw(g);
		bounceBar.Draw(g);
		goalBar.Draw(g);
		buttonsImg.Draw(g);
		*g << font("PIXELOID.TTF", 31) << center << xy(95, 850) << color(CColor(192, 203, 220)) <<  "Level: " + to_string(GetLevel());

		if (IsPaused()) {
			*g << font(60) << xy(GetWidth() / 2, 650) << color(CColor::White()) << "GAME PAUSED";
		}
		*g << font(29);
		if (playState == END) {
			endMenu.Draw(g);
		}

	}
	cursor.Draw(g);
}

void CMyGame::SaveLevel()
{
	string level = "";
	for (int i = 55; i < Height-1; i++) {
		for (int j = 41; j < Width - 41; j++) {
			level += to_string(grid[i][j].type);
		}
	}
	ofstream file("levels.txt", ios::app);
	file << level << "\n";
	file.close();
}

void CMyGame::LoadLevel(int level) 
{
	int currentLine = 0;
	string line;
	ifstream file;
	file.open("levels.txt");
	while (getline(file, line)) {
		if (currentLine == level) {
			break;
		}
		currentLine++;
	}
	file.close();

	int index = 0;
	for (int i = 55; i < Height - 1; i++) {
		for (int j = 41; j < Width - 41; j++) {
			grid[i][j].type = line[index] - 48;
			index++;
		}
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	mainMenu.LoadImage("mainMenu.png");
	mainMenu.SetImage("mainMenu.png");
	mainMenu.SetPos(GetWidth() / 2, GetHeight() / 2);
	background.LoadImage("background4.png");
	background.SetImage("background4.png");
	background.SetPos(GetWidth() / 2, GetHeight() / 2);
	gameMenu.LoadImage("gameMenu.png");
	gameMenu.SetImage("gameMenu.png");
	gameMenu.SetPos(GetWidth() / 2, GetHeight() / 2);
	buttonsImg.LoadImage("buttons&Bars.png");
	buttonsImg.SetImage("buttons&Bars.png");
	buttonsImg.SetPos(GetWidth() / 2, GetHeight() / 2);
	endMenu.LoadImage("endMenu.png");
	endMenu.SetImage("endMenu.png");
	endMenu.SetPos(GetWidth() / 2, GetHeight() / 2);

	mainButtons.push_back(new CSpriteRect(1699, 98, 297, 145, CColor::White(), 0));

	mainButtons.push_back(new CSpriteRect(310, 644, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(635, 644, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(960, 644, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(1283, 644, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(1609, 644, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(310, 379, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(635, 379, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(960, 379, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(1285, 379, 300, 175, CColor::White(), 0));
	mainButtons.push_back(new CSpriteRect(1610, 379, 300, 175, CColor::White(), 0));

	gameButtons.push_back(new CSpriteRect(100, 987, 115, 115, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(1822, 987, 115, 115, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(1822, 854, 115, 115, CColor::White(), 0));

	gameButtons.push_back(new CSpriteRect(234, 171, 115, 100, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(391, 171, 115, 100, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(554, 171, 115, 100, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(1530, 170, 115, 100, CColor::White(), 0));
	gameButtons.push_back(new CSpriteRect(1702, 169, 115, 100, CColor::White(), 0));

	endButtons.push_back(new CSpriteRect(961, 639, 298, 146, CColor::White(), 0));
	endButtons.push_back(new CSpriteRect(761, 476, 295, 145, CColor::White(), 0));
	endButtons.push_back(new CSpriteRect(1162, 476, 294, 145, CColor::White(), 0));

	woodBar.SetColor(CColor(96, 60, 17));
	bounceBar.SetColor(CColor::Red());
	
	Height = 200;
	Width = 400;
	// https://stackoverflow.com/questions/936687/how-do-i-declare-a-2d-array-in-c-using-new
	grid = new gridspace*[Height];
	for (int i = 0; i < Height; i++)
		grid[i] = new gridspace[Width];

	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {

			gridspace point; point.i = i; point.j = j; point.type = 0; point.velx = 0; point.vely = 0; point.rotation = 0; point.bouncing = false; point.time = 0;
			//if (rand() % 100 > 98) point.type = 1;
			if (i < 55 || i > Height - 2 || j < 41 || j > Width - 42) point.type = 4;

			grid[i][j] = point;
		}
	}

	music.Play("music.wav", -1);
	music.Volume(0.4);

	HideMouse();
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	PauseGame(false);
	music.Resume();
	previousBlock = nullptr;

	playState = MAIN;

	selectedShape = FREEHAND;
	selectedShapeRect.SetPos(gameButtons[3]->GetPos());
	selectedBlock = WOOD;
	selectedBlockRect.SetPos(gameButtons[6]->GetPos());
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	
	
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	if (nLevel > 0) {
		PauseGame(false);
		music.Resume();
		playState = PLAY;
		woodBar.SetProgress(100);
		bounceBar.SetProgress(100);
		goalBar.SetProgress(0);

		LoadLevel(nLevel - 1);
		switch (nLevel - 1)
		{
		case 0:
			generatorPos = CVector(130, 180);
			generatorBlock = SAND;
			goalBlock = SAND;
			amountOfWood = 600;
			amountOfBounce = 0;
			bounceBar.SetProgress(0);
			break;
		case 1:
			generatorPos = CVector(140, 190);
			generatorBlock = SAND;
			goalBlock = SAND;
			amountOfWood = 800;
			amountOfBounce = 0;
			bounceBar.SetProgress(0);
			break;
		case 2:
			generatorPos = CVector(240, 193);
			generatorBlock = SAND;
			goalBlock = SAND;
			amountOfWood = 300;
			amountOfBounce = 300;
			break;
		case 3:
			generatorPos = CVector(170, 190);
			generatorBlock = SAND;
			goalBlock = SAND;
			amountOfWood = 300;
			amountOfBounce = 1000;
			break;
		case 4:
			generatorPos = CVector(110, 195);
			generatorBlock = SAND;
			goalBlock = SAND;
			amountOfWood = 600;
			amountOfBounce = 600;
			break;
		case 5:
			generatorPos = CVector(100, 180);
			generatorBlock = WATER;
			goalBlock = WATER;
			amountOfWood = 1000;
			amountOfBounce = 0;
			bounceBar.SetProgress(0);
			break;
		case 6:
			generatorPos = CVector(250, 190);
			generatorBlock = WATER;
			goalBlock = WATER;
			amountOfWood = 800;
			amountOfBounce = 600;
			break;
		case 7:
			generatorPos = CVector(162, 180);
			generatorBlock = WATER;
			goalBlock = GREEN_WATER;
			amountOfWood = 600;
			amountOfBounce = 600;
			break;
		case 8:
			generatorPos = CVector(280, 195);
			generatorBlock = WATER;
			goalBlock = GREEN_WATER;
			amountOfWood = 1000;
			amountOfBounce = 600;
			break;
		case 9:
			generatorPos = CVector(122, 180);
			generatorBlock = WATER;
			goalBlock = WATER;
			amountOfWood = 1000;
			amountOfBounce = 600;
			break;
		}
		switch (goalBlock)
		{
		case SAND:
			goalBar.SetColor(CColor::Yellow());
			break;
		case WATER:
			goalBar.SetColor(CColor::Blue());
			break;
		case GREEN_WATER:
			goalBar.SetColor(CColor::Green());
			break;
		}
	}
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE) {
		if (IsPaused()) music.Resume();
		else music.Pause();
		PauseGame();
	}
	if (sym == SDLK_F2)
		NewGame();

	//if (sym == SDLK_k) {
	//	selectedBlock = ENDBLOCK;
	//}
	//if (sym == SDLK_l) {
	//	selectedBlock = GREEN_CHANGER;
	//}
	//if (sym == SDLK_s) {
	//	//SaveLevel();
	//}

}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers



void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	cursor.SetPos(x, y);
	if (playState == MAIN) {
		bool selected = false;
		for (int i = 1; i < mainButtons.size(); i++) {
			if (mainButtons[i]->HitTest(x, y)) {
				levelSelect.SetPos(mainButtons[i]->GetPos()-CVector(0, 4));
				selected = true;
				break;
			}
		}
		if (!selected)
			levelSelect.SetPos(-400, -400);
	}
	// If the player started drawing, it will check if it can draw
	else if (bLeft && previousBlock != nullptr && !IsPaused() && playState == PLAY) {
		switch (selectedBlock)
		{
		case WOOD:
			if (woodBar.GetProgress() <= 0) return;
			break;
		case BOUNCE:
			if (bounceBar.GetProgress() <= 0) return;
			break;
		default:
			break;
		}
		// First finds where the mouse position is located
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width; j++) {
				if (x > (j - 0.5) * GetWidth() / Width && y > (i - 0.5) * GetHeight() / Height && x <= (j + 0.5) * GetWidth() / Width && y <= (i + 0.5) * GetHeight() / Height) {
					// Finds all point between this new position and the prvious position for free hand, and starting position for line and square tool
					vector<gridspace*> points = GetBetweenPoints(previousBlock, &grid[i][j]);
					// Finds the rotation of this line
					float rotation = 0;
					if (points.size() != 0) {
						rotation = atan((float)(points[points.size() - 1]->i - previousBlock->i) / (float)(points[points.size() - 1]->j - previousBlock->j));
						cout << RAD2DEG(rotation) << endl;
					}
					switch (selectedShape)
					{
					case FREEHAND:
						// Sets the new position to the starting position for the next time the mouse moves
						previousBlock = &grid[i][j];
						break;
					case LINE:
						// Removes the previous line
						for (gridspace* point : currentShape) {
							point->type = 0;
							point->rotation = 0;
						}
						currentShape.clear();
						break;
					case SQUARE:
						// Removes the previous square
						for (gridspace* point : currentShape) {
							point->type = 0;
							point->rotation = 0;
						}
						currentShape.clear();

						vector<gridspace*> rectPoints;
						gridspace* point1;
						gridspace* point2;
						// Finds the lowest point
						if (previousBlock->i < grid[i][j].i) {
							point1 = previousBlock;
							point2 = &grid[i][j];
						}
						else {
							point1 = &grid[i][j];
							point2 = previousBlock;
						}
						// using this it draws two vertical lines
						for (int yIndex = point1->i; yIndex <= point2->i; yIndex++) {
							rectPoints.push_back(&grid[yIndex][point1->j]);
							rectPoints.push_back(&grid[yIndex][point2->j]);
						}
						// Finds the point most to the left
						if (previousBlock->j < grid[i][j].j) {
							point1 = previousBlock;
							point2 = &grid[i][j];
						}
						else {
							point1 = &grid[i][j];
							point2 = previousBlock;
						}
						// using this it draws two vertical lines
						for (int xIndex = point1->j; xIndex <= point2->j; xIndex++) {
							rectPoints.push_back(&grid[point1->i][xIndex]);
							rectPoints.push_back(&grid[point2->i][xIndex]);
						}
						points = rectPoints;
						rotation = 0;
						break;
					}
					// Draws the new points that the program calculated this time the mouse moved
					for (auto point : points) {
						for (int y = point->i - 1; y <= point->i + 1; y++) {
							for (int x = point->j - 1; x <= point->j + 1; x++) {
								if (y >= 0 && y < Height && x >= 0 && x < Width && grid[y][x].type == 0) {
									grid[y][x].type = selectedBlock;
									grid[y][x].rotation = rotation;
									if (selectedShape == FREEHAND) {
										switch (selectedBlock)
										{
										case WOOD:
											woodBar.SetProgress(woodBar.GetProgress() - 100.f / amountOfWood);
											break;
										case BOUNCE:
											bounceBar.SetProgress(bounceBar.GetProgress() - 100.f / amountOfBounce);
											break;
										default:
											break;
										}
									}
									else {
										currentShape.push_back(&grid[y][x]);
									}
									if (drawSound.IsPaused()) drawSound.Resume();
								}
							}
						}

					}
					break;
				}
			}
		}
	}
}


void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	switch (playState) {
	case MAIN:
		for (int i = 0; i < mainButtons.size(); i++) {
			if (mainButtons[i]->HitTest(x, y)) {
				if (i == 0) {
					StopGame();
				}
				else {
					SetLevel(i);
					playState = PLAY;
					levelSelect.SetPos(-400, -400);
				}
				buttonsSound.Play("button.wav");
				break;
			}
		}
		break;
	case PLAY:
		for (int i = 0; i < gameButtons.size(); i++) {
			if (gameButtons[i]->HitTest(x, y)) {
				switch (i)
				{
				case 0:
					NewGame();
					break;
				case 1:
					if (IsPaused()) music.Resume();
					else music.Pause();
					PauseGame();
					break;
				case 2:
					OnStartLevel(GetLevel());
					break;
						
				case 3:
					selectedShape = FREEHAND;
					selectedShapeRect.SetPos(gameButtons[i]->GetPos());
					break;
				case 4:
					selectedShape = LINE;
					selectedShapeRect.SetPos(gameButtons[i]->GetPos());
					break;
				case 5:
					selectedShape = SQUARE;
					selectedShapeRect.SetPos(gameButtons[i]->GetPos());
					break;

				case 6:
					selectedBlock = WOOD;
					selectedBlockRect.SetPos(gameButtons[i]->GetPos());
					break;
				case 7:
					selectedBlock = BOUNCE;
					selectedBlockRect.SetPos(gameButtons[i]->GetPos());
					break;
				}
				buttonsSound.Play("button.wav");
			}
		}
		// Checks if the player can draw
		if (!IsPaused()) {
			switch (selectedBlock)
			{
			case WOOD:
				if (woodBar.GetProgress() <= 0) return;
				drawSound.Play("wood.wav", -1);
				drawSound.Volume(0.8);
				drawSound.Pause();
				break;
			case BOUNCE:
				if (bounceBar.GetProgress() <= 0) return;
				drawSound.Play("bouncy.wav", -1);
				drawSound.Volume(0.3);
				drawSound.Pause();
				break;
			default:
				break;
			}
			// Creates a starting dot and sets this point as the starting point of the shape
			for (int i = 0; i < Height; i++) {
				for (int j = 0; j < Width; j++) {
					if (x > (j - 0.5) * GetWidth() / Width && y > (i - 0.5) * GetHeight() / Height && x <= (j + 0.5) * GetWidth() / Width && y <= (i + 0.5) * GetHeight() / Height) {
						previousBlock = &grid[i][j];
						for (int y = i - 1; y <= i + 1; y++) {
							for (int x = j - 1; x <= j + 1; x++) {
								if (y >= 0 && y < Height && x >= 0 && x < Width && grid[y][x].type == 0) {
									grid[y][x].type = selectedBlock;
									drawSound.Resume();
									switch (selectedBlock)
									{
									case WOOD:
										woodBar.SetProgress(woodBar.GetProgress() - 100.f / amountOfWood);
										break;
									case BOUNCE:
										bounceBar.SetProgress(bounceBar.GetProgress() - 100.f / amountOfBounce);
										break;
									default:
										break;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
		break;
	case END:
		for (int i = 0; i < endButtons.size(); i++) {
			if (endButtons[i]->HitTest(x, y)) {
				buttonsSound.Play("button.wav");
				switch (i) {
				case 0:
					//Next Level
					if (GetLevel() < 10)
						SetLevel(GetLevel() + 1);
					break;
				case 1:
					NewGame();
					break;
				case 2:
					OnStartLevel(GetLevel());
					break;
				}
			}
		}
		break;
	}
}

void CMyGame::OnLButtonUp(Uint16 x, Uint16 y)
{
	switch (selectedBlock)
	{
	case WOOD:
		woodBar.SetProgress(woodBar.GetProgress() - 100.f / amountOfWood * (float)currentShape.size());
		break;
	case BOUNCE:
		bounceBar.SetProgress(bounceBar.GetProgress() - 100.f / amountOfBounce * (float)currentShape.size());
		break;
	default:
		break;
	}
	currentShape.clear();
	previousBlock = nullptr;
	drawSound.Pause();
}

void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x, Uint16 y)
{
}
