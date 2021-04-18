#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <sys/fcntl.h>      
#include <sys/ioctl.h>      
#include <unistd.h>         /*  Symbolic Constants  */
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char *argv[])
{
    unsigned short key;
    int fd, ret;

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    ioctl(fd, KEY_IOCTL_CLEAR, key);
    while(1)
    {
        ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        
        if(ret < 0)
        {
            sleep(1);
            continue;
        }

        ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);

        if((key & 0xff) == '#')
            break;

        if((char)key == 'A')
            printf("A\n");
    }

    close(fd);

    return 0;
}
