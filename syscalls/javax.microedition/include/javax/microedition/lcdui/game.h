/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      game.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   lcdui package
 *
 * $Id: game.h 12478 2006-11-26 14:24:52Z ska $
 *
 ********************************************************************/
#ifndef __JAVAX__MICROEDITION__LCDUI__GAME_H__
#define __JAVAX__MICROEDITION__LCDUI__GAME_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <javax/microedition/lcdui.h>

/* From GameCanvas class */
#define NOPH_GameCanvas_DOWN_PRESSED   0x0040
#define NOPH_GameCanvas_UP_PRESSED     0x0002
#define NOPH_GameCanvas_LEFT_PRESSED   0x0004
#define NOPH_GameCanvas_RIGHT_PRESSED  0x0020
#define NOPH_GameCanvas_FIRE_PRESSED   0x0100
#define NOPH_GameCanvas_GAME_A_PRESSED 0x0200
#define NOPH_GameCanvas_GAME_B_PRESSED 0x0400
#define NOPH_GameCanvas_GAME_C_PRESSED 0x0800
#define NOPH_GameCanvas_GAME_D_PRESSED 0x1000

typedef int NOPH_GameCanvas_t;
typedef int NOPH_Sprite_t;
typedef int NOPH_TiledLayer_t;
typedef int NOPH_Layer_t;
typedef int NOPH_LayerManager_t;

NOPH_GameCanvas_t NOPH_GameCanvas_get(void); /* Not generated */
void NOPH_GameCanvas_flushGraphics(NOPH_GameCanvas_t canvas);
void NOPH_GameCanvas_flushGraphics_rect(NOPH_GameCanvas_t canvas, int x, int y, int width, int height);
NOPH_Graphics_t NOPH_GameCanvas_getGraphics(NOPH_GameCanvas_t canvas); /* Not generated */
int NOPH_GameCanvas_getWidth(NOPH_GameCanvas_t canvas);
int NOPH_GameCanvas_getHeight(NOPH_GameCanvas_t canvas);
int NOPH_GameCanvas_getKeyStates(NOPH_GameCanvas_t canvas);

/* Sprite class */
NOPH_Sprite_t NOPH_Sprite_new(NOPH_Image_t image);
NOPH_Sprite_t NOPH_Sprite_new_imageSize(NOPH_Image_t image, int width, int height);
NOPH_Sprite_t NOPH_Sprite_new_sprite(NOPH_Sprite_t sprite);
void NOPH_Sprite_setFrame(NOPH_Sprite_t sprite, int sequenceIndex);
void NOPH_Sprite_setImage(NOPH_Sprite_t sprite, NOPH_Image_t image, int frameWidth, int frameHeight);
void NOPH_Sprite_paint(NOPH_Sprite_t sprite, NOPH_Graphics_t graphics);
#define NOPH_Sprite_move(layer, dx, dy) NOPH_Layer_move(layer, dx, dy)
#define NOPH_Sprite_setPosition(layer, x, y) NOPH_Layer_setPosition(layer, x, y)
#define NOPH_Sprite_setVisible(layer, visible) NOPH_Layer_setVisible(layer, visible)
#define NOPH_Sprite_getWidth(layer) NOPH_Layer_getWidth(layer)
#define NOPH_Sprite_getHeight(layer) NOPH_Layer_getHeight(layer)

/* LayerManager, layer and TiledLayer classes */
void NOPH_Layer_move(NOPH_Layer_t layer, int dx, int dy);
void NOPH_Layer_setPosition(NOPH_Layer_t layer, int x, int y);
void NOPH_Layer_setVisible(NOPH_Layer_t layer, bool_t visible);
int NOPH_Layer_getWidth(NOPH_Layer_t layer);
int NOPH_Layer_getHeight(NOPH_Layer_t layer);

NOPH_TiledLayer_t NOPH_TiledLayer_new(int columns, int rows, NOPH_Image_t image, int tileWidth, int tileHeight);
void NOPH_TiledLayer_setCell(NOPH_TiledLayer_t layer, int col, int row, int tileIndex);
int NOPH_TiledLayer_getCell(NOPH_TiledLayer_t layer, int col, int row);
void NOPH_TiledLayer_paint(NOPH_TiledLayer_t layer, NOPH_Graphics_t graphics);
#define NOPH_TiledLayer_move(layer, dx, dy) NOPH_Layer_move(layer, dx, dy)
#define NOPH_TiledLayer_setPosition(layer, x, y) NOPH_Layer_setPosition(layer, x, y)
#define NOPH_TiledLayer_setVisible(layer, visible) NOPH_Layer_setVisible(layer, visible)
#define NOPH_TiledLayer_getWidth(layer) NOPH_Layer_getWidth(layer)
#define NOPH_TiledLayer_getHeight(layer) NOPH_Layer_getHeight(layer)

NOPH_LayerManager_t NOPH_LayerManager_new(void);
void NOPH_LayerManager_append(NOPH_LayerManager_t mgr, NOPH_Layer_t layer);
void NOPH_LayerManager_paint(NOPH_LayerManager_t mgr, NOPH_Graphics_t graphics, int x, int y);
void NOPH_LayerManager_setViewWindow(NOPH_LayerManager_t mgr, int x, int y, int width, int height);

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVAX__MICROEDITION__LCDUI__GAME_H__ */
