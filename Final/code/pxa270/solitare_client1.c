#include "solitare_func.h"

int main(int argc, char *argv[])
{
    int connfd; /* socket descriptor */
    char snd[1024], rcv[1024];
    int cover_place = 0, card_place = -1;
    int n;

    connfd = connectsock(argv[1], argv[2], "tcp");

    unsigned short key;
    unsigned short led;
    lcd_write_info_t lcd;
    _7seg_info_t _7seg;

    int fd, ret_key;

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    Clear_IO(fd, lcd, _7seg, key, led);
    memset(lcd.Msg, 0, sizeof(lcd.Msg));
    lcd.Count = 0;

    n = sprintf(snd, "player1");
    write_to(connfd, snd, n);
    read_from(connfd, rcv);
    display_7seg(fd, _7seg, atoi(rcv));
            
    while(1)
    {
        ret_key = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        
        if(ret_key < 0)
        {
            continue;
        }
        else
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);

        ret_key = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        
        if((char)key == '*')
        {
            card_place = 6;
            printf("%d\n", card_place);
        }
        if((char)key == '0')
        {
            card_place = 7;
            printf("%d\n", card_place);
        }
        if((char)key == '#')
        {
            card_place = 8;
            printf("%d\n", card_place);
        }
        if((char)key == 'D')
        {
            card_place = 9;
            printf("%d\n", card_place);
        }
        if((char)key == '8')
        {
            cover_place = 4;
            printf("%d\n", cover_place);
        }
        if((char)key == '9')
        {
            cover_place = 5;
            printf("%d\n", cover_place);
        }

        lcd.Msg[lcd.Count] = key;
        lcd.Count++;
        display_lcd(fd, lcd);

        if(cover_place != 0 && card_place != -1)
        {
            n = sprintf(snd, "%d%d", card_place, cover_place);
            write_to(connfd, snd, n);
            cover_place = 0;
            card_place = -1;
            read_from(connfd, rcv);
            display_7seg(fd, _7seg, atoi(rcv));
        }
    }    
    
    /* close client socket */
    close(connfd);

    return 0;
}
