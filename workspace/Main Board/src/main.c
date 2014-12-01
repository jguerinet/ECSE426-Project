 /*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "background16bpp.h"

#include <stdio.h>
#include <string.h>


static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--)
  {
  }
}

double x_c = 3.5;
double y_c = 2.5;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief    Illustartes a simple animation program
  * @function This function draws concentrated circles emanating from the center towards the LCD edge in
              an animated fashion. It will look as a sonar or radar display. Then it simulates locking 
              onto a target by flashing a small red circle and displaying the text "Object located"
  * @param    None
  * @retval   None
  */

void locate(void const *argument){
	while(1){
			/* Clear the LCD */ 
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DisplayStringLine(LINE(1), (uint8_t*)"Tracking");
		
		
		int i = 0;
		int j = 0;
		
		LCD_SetTextColor(LCD_COLOR_BLUE2);

		
		for (i=10; i <= 310; i = i+60){
			LCD_DrawLine(10, i, 220, LCD_DIR_HORIZONTAL);
		}

		for (j=10; j <= 230; j = j+44){
			LCD_DrawLine(j, 10, 300, LCD_DIR_VERTICAL);
		}
		
		double x_mapped = (x_c+(10/44))*44;
		double y_mapped = (y_c+(10/60))*60;
		
		if (x_mapped < 220 & y_mapped < 300){
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(x_mapped,y_mapped,4);
			delay(125);
		}
		else{
			LCD_Clear(LCD_COLOR_WHITE);
			LCD_SetFont(&Font8x8);
			LCD_DisplayStringLine(LINE(1), (uint8_t*)"Out of Bounds");
		}
		
		osDelay(250);
	}
}


osThreadDef(locate, osPriorityNormal, 1, 0);

// ID for theads

osThreadId locate_thread;

/*
 * main: initialize and start the system
 */

int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here
	/* LCD initiatization */
  LCD_Init();
  
  /* LCD Layer initiatization */
  LCD_LayerInit();
    
  /* Enable the LTDC controler */
  LTDC_Cmd(ENABLE);
  
  /* Set LCD foreground layer as the current layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	/*******************************************************
	         Uncomment the example you want to see
	example_1a: Simple shape draw, fill and text display
	example_1b: bitmap image display
	example_1c: Simple animation
	********************************************************/
	
	//example_1a_thread = osThreadCreate(osThread(example_1a), NULL);
	//example_1b_thread = osThreadCreate(osThread(example_1b), NULL);
	locate_thread = osThreadCreate(osThread(locate), NULL);
	
	osKernelStart ();                         // start thread execution 
}


