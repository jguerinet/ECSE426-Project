#include "stm32f4xx.h"                  		// Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "background16bpp.h"

#include <stdio.h>
#include <string.h>

void drawGrid(void);

/**
	Initializes the LCD screen to a blank screen
*/
void initializeLCD(void){
	//LCD Initialization
  LCD_Init();
  //LCD Layer Initialization
  LCD_LayerInit();
  //Enable the LTDC controller
  LTDC_Cmd(ENABLE);
  //Set LCD foreground layer as the current layer
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	//Clear the LCD
	LCD_Clear(LCD_COLOR_WHITE);
	LCD_SetFont(&Font8x8);
	
	//Initial Grid
	drawGrid(); 
}

/**
	Clears a previous position from the screen 
	@param x The x coordinate
	@param y The y coordinate
*/
void clearPosition(int16_t x, int16_t y){
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DrawFullCircle(x, y, 4);
}

/**
	Draws a red circle at the given permission
	@param x The x coordinate
	@param y The y coordinate
*/
void drawPosition(int16_t x, int16_t y){
	//Set the color to red
	LCD_SetTextColor(LCD_COLOR_RED);
	//Draw a circle at the given location		
	LCD_DrawFullCircle(x, y, 4);
}

/**
	Draws the grid on the screen
*/
void drawGrid(void){
	//Draw the grid in blue
	LCD_SetTextColor(LCD_COLOR_BLUE2);
	int i = 0;
	int j = 0;
	//Horizontal Lines
	for (i = 8; i <= 312; i = i+38){
		LCD_DrawLine(8, i, 224, LCD_DIR_HORIZONTAL);
	}
	//Vertical lines 
	for (j = 8; j <= 232; j = j+28){
		LCD_DrawLine(j, 8, 304, LCD_DIR_VERTICAL);
	}
}
