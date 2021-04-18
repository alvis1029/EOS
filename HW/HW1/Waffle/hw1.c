#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <string.h>
#include <sys/fcntl.h>      
#include <sys/ioctl.h>      
#include <unistd.h>         /*  Symbolic Constants  */
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

void Clear_IO(int fd, lcd_write_info_t lcd, _7seg_info_t _7seg, unsigned short key, unsigned short led)
{
    led = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &led);
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    ioctl(fd, KEY_IOCTL_CLEAR, key);
    ioctl(fd, _7SEG_IOCTL_ON, NULL);
    _7seg.Mode = _7SEG_MODE_HEX_VALUE;
    _7seg.Which = _7SEG_ALL;
    _7seg.Value = 0;
    ioctl(fd, _7SEG_IOCTL_SET, &_7seg);
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
}

void display_7seg(int fd, _7seg_info_t _7seg, int ans)
{
    _7seg.Mode = _7SEG_MODE_HEX_VALUE;
    _7seg.Which = _7SEG_ALL;
    _7seg.Value = (ans/100)*256 + ((ans%100)/10)*16 + ans%10;
    ioctl(fd, _7SEG_IOCTL_SET, &_7seg);
}

void display_lcd(int fd, lcd_write_info_t lcd)
{
    ioctl(fd, LCD_IOCTL_WRITE, &lcd);
}

void main_menu(int fd, lcd_write_info_t lcd)
{
    char screen[] = "1. menu\n2. order\n";

    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    lcd.Count = sprintf((char *)lcd.Msg, screen);
    display_lcd(fd, lcd);
}

void menu(int fd, lcd_write_info_t lcd)
{
    char screen[] = "pancake   $40\nblack tea $30\n";

    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    lcd.Count = sprintf((char *)lcd.Msg, screen);
    display_lcd(fd, lcd);
}

void show(int fd, lcd_write_info_t lcd, int p, int b)
{
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    lcd.Count = sprintf((char *)lcd.Msg, "pancake   %d\nblack tea %d\n\n$%d|%ds", p, b, p*40+b*30, p*5>=b*3?p*5:b*3);
    display_lcd(fd, lcd);
}

void order(int fd, lcd_write_info_t lcd)
{
    char screen[] = "1. show\n2. pancake\n3. black tea\n4. check out\n5. cancel\n";
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    lcd.Count = sprintf((char *)lcd.Msg, screen);
    display_lcd(fd, lcd);
}

void check_out(int fd, lcd_write_info_t lcd, int c_n, int p, int b, int p_f, int b_f)
{
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    lcd.Count = sprintf((char *)lcd.Msg, "No.%04d\npancake   %d/%d\nblack tea %d/%d\nfee: $%d", c_n, p_f, p, b_f, b, p*40+b*30);
    display_lcd(fd, lcd);
}

void display_order(int fd, lcd_write_info_t lcd, int x)
{
    if(x != 0)
    {
        lcd.Count = sprintf((char *)lcd.Msg, "%d", x);
        display_lcd(fd, lcd);
    }
}

int main(int argc, char *argv[])
{
    unsigned short key;
    unsigned short led;
    unsigned short data;
    lcd_write_info_t lcd;
    _7seg_info_t _7seg;

    int fd, ret_key, cur_page = 0, p_num = 0, b_num = 0, costum_num = 1, p_fin_num = -1, b_fin_num = -1;
    int timer = -1, start_cnt = 0, p_temp = 0, b_temp = 0;

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    Clear_IO(fd, lcd, _7seg, key, led);
    main_menu(fd, lcd);

    while(1)
    {
        ret_key = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);

        if(ret_key < 0)
            continue;
        
        ret_key = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        
        if((char)key == '1')
        {
            if(cur_page == 0)
            {
                menu(fd, lcd);
                cur_page = 1;
            }
            else if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 2)
            {
                show(fd, lcd, p_num, b_num);
                cur_page = 3;
            }
            else if(cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += 1;
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += 1;
                display_order(fd, lcd, b_temp);
            }
        }
        else if((char)key == '2')
        {
            if(cur_page == 0 || cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 2)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                cur_page = 4;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += 2;
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += 2;
                display_order(fd, lcd, b_temp);
            }
        }
        else if((char)key == '3')
        {
            if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 2)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                cur_page = 5;
            }
            else if(cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += 3;
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += 3;
                display_order(fd, lcd, b_temp);
            }
        }
        else if((char)key == '4')
        {
            if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 2)
            {
                start_cnt = 1;
                cur_page = 6;
            }
            else if(cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += 4;
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += 4;
                display_order(fd, lcd, b_temp);
            }
        }
        else if((char)key == '5')
        {
            if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 2)
            {
                main_menu(fd, lcd);
                cur_page = 0;
                p_num = 0;
                b_num = 0;
                p_temp = 0;
                b_temp = 0;
                p_fin_num = -1;
                b_fin_num = -1;
                start_cnt = 0;
                timer = -1;
            }
            else if(cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += 5;
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += 5;
                display_order(fd, lcd, b_temp);
            }
        }
        else if((char)key == '#')
        {
            if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            if(cur_page == 3 || cur_page == 4 || cur_page == 5)
            {
                order(fd, lcd);
                cur_page = 2;
                p_num += p_temp;
                b_num += b_temp;
                p_temp = 0;
                b_temp = 0;
            }
        }
        else
        {
            if(cur_page == 1)
            {
                main_menu(fd, lcd);
                cur_page = 0;
            }
            else if(cur_page == 3)
            {
                order(fd, lcd);
                cur_page = 2;
            }
            else if(cur_page == 4)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                p_temp *= 10;
                p_temp += (char)key - '0';
                display_order(fd, lcd, p_temp);
            }
            else if(cur_page == 5)
            {
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                b_temp *= 10;
                b_temp += (char)key - '0';
                display_order(fd, lcd, b_temp);
            }
        }

        while(start_cnt)
        {
            timer++;
            printf("timer = %d\n", timer);

            if(timer%5 == 0 && p_fin_num < p_num)
                p_fin_num++;
            if(timer%3 == 0 && b_fin_num < b_num)
                b_fin_num++;
            check_out(fd, lcd, costum_num, p_num, b_num, p_fin_num, b_fin_num);

            if(p_fin_num < p_num)
            {
                switch(timer%5)
                {
                    case 0:
                    {
                        data = LED_D9_INDEX;
                        break;
                    }
                    case 1:
                    {
                        data = LED_D10_INDEX;
                        break;
                    }
                    case 2:
                    {
                        data = LED_D11_INDEX;
                        break;
                    }
                    case 3:
                    {
                        data = LED_D12_INDEX;
                        break;
                    }
                    case 4:
                    {
                        data = LED_D13_INDEX;
                        break;
                    }
                }
                ioctl(fd, LED_IOCTL_BIT_SET, &data);
            }

            if(b_fin_num < b_num)
            {
                switch(timer%3)
                {
                    case 0:
                    {
                        data = LED_D14_INDEX;
                        break;
                    }
                    case 1:
                    {
                        data = LED_D15_INDEX;
                        break;
                    }
                    case 2:
                    {
                        data = LED_D16_INDEX;
                        break;
                    }
                }
                ioctl(fd, LED_IOCTL_BIT_SET, &data);
            }

            if(p_fin_num == p_num && b_fin_num == b_num)
            {
                main_menu(fd, lcd);
                display_7seg(fd, _7seg, costum_num);
                data = LED_ALL_OFF;
                ioctl(fd, LED_IOCTL_SET, &data);
                costum_num++;
                cur_page = 0;
                p_num = 0;
                b_num = 0;
                p_temp = 0;
                b_temp = 0;
                p_fin_num = 0;
                b_fin_num = 0;
                start_cnt = 0;
                timer = -1;
                break;
            }

            sleep(1);

            if((timer > 0 && timer%5 == 4) || p_fin_num >= p_num)
            {
                data = LED_D9_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D10_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D11_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D12_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D13_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
            }

            if((timer > 0 && timer%3 == 2) || b_fin_num >= b_num)
            {
                data = LED_D14_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D15_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
                data = LED_D16_INDEX;
                ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
            }
        }
    }

    close(fd);

    return 0;
}
