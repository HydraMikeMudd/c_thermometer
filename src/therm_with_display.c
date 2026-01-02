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


	
	// Partial refresh
	Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);  
    Debug("Partial refresh\r\n");
    Paint_SelectImage(BlackImage);

	// Writing static text
    Paint_DrawString_EN(20, 20, "Temp: 52.76 F", &Font24, BLACK, WHITE);
	
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 10;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
        Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
		EPD_2in13_V4_Display_Partial(BlackImage);
        DEV_Delay_ms(500);//Analog clock 1s
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

