#include "stdafx.h"
#include "SpriteBar.h"



void SpriteBar::init(bool isVertical)
{
	if (isVertical) {
		SetPivotFromCenter(-GetW() / 2);
		maxWidth = GetW();
	}
	else {
		SetPivotFromCenter(0, -GetH() / 2);
		maxHeight = GetH();
	}
}

void SpriteBar::SetColor(CColor color)
{
	barColor = color;
	SetColorKey(CColor::AnyBut(color));
	
}

void SpriteBar::SetProgress(float numbProgress)
{
	if (numbProgress > 0.0001)
		progress = numbProgress;
	else {
		progress = 0;
		numbProgress = 0.0001;
	}

	float newLength = numbProgress / 100.f * max(maxWidth, maxHeight);
	if (maxWidth != 0)
		SetSize(newLength, GetHeight());
	else
		SetSize(GetWidth(), newLength);
	
}

float SpriteBar::GetProgress() {
	return progress;
}

void SpriteBar::Draw(CGraphics* g)
{
	if (IsValid()) return;
	CRectangle rect;
	GetBoundingRect(rect);
	g->FillRect(rect, barColor); 
}
