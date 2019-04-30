#ifndef DRAW_H
#define DRAW_H

#include "shared.h"

void InitDraw();
void QuitDraw();
void ClearDrawScreen();
void DrawPixels(int_8 x, int_8 y, int_8* bits_to_draw, int_8 height);

#endif