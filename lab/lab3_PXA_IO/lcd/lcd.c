#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <sys/fcntl.h>      
#include <sys/ioctl.h>      
#include <unistd.h>         /*  Symbolic Constants  */
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char *argv[])
{
    int fd;
    lcd_write_info_t display;   /* struct for saving LCD data */

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    /* Clear LCD */
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);

    /* Save output string to display data structure */
    display.Count = sprintf((char *)display.Msg, "Hello World\n");

    /* Print out "Hello World to LCD */
    ioctl(fd, LCD_IOCTL_WRITE, &display);

    /* Get the cursor position */
    ioctl(fd, LCD_IOCTL_CUR_GET, &display);
    printf("The cursor position is at (x, y) = (%d, %d)\n", display.CursorX, display.CursorY);
    
    close(fd);

    return 0;
}
