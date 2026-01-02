#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "Debug.h"
#include <stdlib.h>
#include "EPD_2in13_V4.h"
#include <time.h>

// Screen Width: 122
// Screen Height: 250
// Font 20 Width: 14
// Font 20 Height: 20
// Font 24 Width: 17
// Font 24 height: 24

int main(void)
{

    Debug("EPD_2in13 Thermometer\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    Debug("e-Paper Init and Clear...\r\n");
	EPD_2in13_V4_Init();


	struct timespec start={0,0}, finish={0,0}; 
    clock_gettime(CLOCK_REALTIME,&start);
    EPD_2in13_V4_Clear();
	clock_gettime(CLOCK_REALTIME,&finish);
    Debug("%ld S\r\n",finish.tv_sec-start.tv_sec);	

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }
    Debug("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);


	
	Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);  
    Debug("Partial refresh\r\n");
    Paint_SelectImage(BlackImage);

	// Writing static text
    Paint_DrawString_EN(20, 20, "Temp: ", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(20, 40 + Font24.Height, "Hum:  ", &Font24, BLACK, WHITE);	
    Paint_DrawString_EN(20 + Font24.Width * 11, 20, " F", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(20 + Font24.Width * 11, 40 + Font24.Height, " %", &Font24, BLACK, WHITE);

    UBYTE num = 10;
	float temp = 72.75f;
	float humidity = 7.24f;

	char temp_buffer[6];
	char humidity_buffer[6];

    for (;;) {
        Paint_ClearWindows(20 + (Font24.Width * 6), 20, 20 + (Font24.Width * 11), 40 + (Font24.Height * 2), WHITE);
		
    	snprintf(temp_buffer, sizeof(temp_buffer), "%.2f", temp);
    	snprintf(humidity_buffer, sizeof(humidity_buffer), "%.2f", humidity);

    	Paint_DrawString_EN(20 + (Font24.Width * 6), 20, temp_buffer, &Font24, BLACK, WHITE);
    	Paint_DrawString_EN(20 + (Font24.Width * 6), 40 + Font24.Height, humidity_buffer, &Font24, BLACK, WHITE);

		temp++;
		humidity++;
        num = num - 1;
        if(num == 0) {
            break;
        }
		EPD_2in13_V4_Display_Partial(BlackImage);
        DEV_Delay_ms(2000);//Analog clock 1s
    }

	// Clearing and closing connection
	Debug("Clear...\r\n");
	EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();
	
    Debug("Goto Sleep...\r\n");
    EPD_2in13_V4_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    Debug("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
    return 0;
}

