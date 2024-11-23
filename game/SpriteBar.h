#pragma once

class SpriteBar :  public CSpriteRect
{
	using CSpriteRect::CSpriteRect;
	float maxWidth = 0;
	float maxHeight = 0;
	CColor barColor;
	float progress;
public:
	void init(bool isVertical=true);
	void SetColor(CColor color);
	void SetProgress(float numbProgess);
	float GetProgress();
	virtual void Draw(CGraphics* g);
};

