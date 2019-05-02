#ifndef DRAW_H
#define DRAW_H

#include "shared.h"

void InitDraw();
void QuitDraw();
void ClearDrawScreen();
void DrawScreen();
void SetPixels(uint_8 x, uint_8 y, uint_8* bits_to_draw, uint_8 height);
uint_8 CollisionDetected();

#endif