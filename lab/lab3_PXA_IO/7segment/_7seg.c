#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <sys/fcntl.h>      
#include <sys/ioctl.h>      
#include <unistd.h>         /*  Symbolic Constants  */
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char *argv[])
{
    _7seg_info_t data;
    int fd, ret, i;

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    ioctl(fd, _7SEG_IOCTL_ON, NULL);
    data.Mode = _7SEG_MODE_HEX_VALUE;
    data.Which = _7SEG_ALL;
    data.Value = 0x2004;
    ioctl(fd, _7SEG_IOCTL_SET, &data);
    sleep(3);

    data.Mode = _7SEG_MODE_PATTERN;
    data.Which = _7SEG_D5_INDEX | _7SEG_D8_INDEX;
    data.Value = 0x6d7f;    /* change to 5008 */
    ioctl(fd, _7SEG_IOCTL_SET, &data);
    sleep(3);

    ioctl(fd, _7SEG_IOCTL_ON, NULL);

    /* Close fd */
    close(fd);

    return 0;
}
