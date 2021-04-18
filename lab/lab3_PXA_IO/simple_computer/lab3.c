#include <stdio.h>          /*  Input/Output  */
#include <stdlib.h>         /*  General Utilities  */
#include <string.h>
#include <sys/fcntl.h>      
#include <sys/ioctl.h>      
#include <unistd.h>         /*  Symbolic Constants  */
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int last_row = 0;

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

void display_led(int fd, unsigned short led)
{
    ioctl(fd, LED_IOCTL_SET, &led);
}

void display_lcd(int fd, lcd_write_info_t lcd)
{
    ioctl(fd, LCD_IOCTL_WRITE, &lcd);
}

int calculate(int fd, lcd_write_info_t lcd, _7seg_info_t _7seg)
{
    int ans = 0, num[20], ind = 0, last_op = last_row-1, i, j, priority[20];
    char tmp[512];

    for(i=last_row; i<lcd.Count; i++)
    {
        if(lcd.Msg[i] == '+' || lcd.Msg[i] == '-' || lcd.Msg[i] == 'x' || lcd.Msg[i] == '/')
        {
            for(j=last_op+1; j<i; j++)
                tmp[j-last_op-1] = lcd.Msg[j];
            tmp[i-last_op-1] = '\0';
            
            num[ind] = atoi(tmp);

            switch(lcd.Msg[i])
            {
                case '+':
                {
                    priority[ind] = 0;
                    break;
                }
                case '-':
                {
                    priority[ind] = 1;
                    break;
                }
                case 'x':
                {
                    priority[ind] = 2;
                    break;
                }
                case '/':
                {
                    priority[ind] = 3;
                    break;
                }
            }
            
            ind ++;
            last_op = i;

            printf("%d %c ", num[ind-1], lcd.Msg[i]);
        }

        if(i == lcd.Count-1)
        {
            for(j=last_op+1; j<i+1; j++)
                tmp[j-last_op-1] = lcd.Msg[j];
            tmp[i+1-last_op-1] = '\0';
            
            num[ind] = atoi(tmp);
            ind ++;

            printf("%d", num[ind-1]);
        }
    }

    for(j=0; j<ind-1; j++)
    {
        if(priority[j] == 3)
        {
            if(ind == 2)
                ans += num[j]/num[j+1];
            else if(j == 0)
            {
                if(priority[j+1] == 0 || priority[j+1] == 1)
                    ans += num[j]/num[j+1];
                else
                    ans += num[j];
            }
            else if(priority[j] == priority[j-1])
            {
                if(j == ind-2)
                    ans /= num[j]*num[j+1];
                else
                    ans /= num[j];
            }
            else if(priority[j-1] == 0)
            {
                if(j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0)
                    ans += num[j]/num[j+1];
                else if(priority[j+1] == 3 || priority[j+1] == 2)
                {
                    int p, tmp_ans = num[j]/num[j+1];
                    for(p=j+1; p<ind-1; p++)
                    {
                        if(priority[p] == 1 || priority[p] == 0)
                            break;
                        if(priority[p] == 3)
                            tmp_ans /= num[p+1];
                        else
                            tmp_ans *= num[p+1];
                    }

                    ans += tmp_ans;
                    j = p;
                }         
                else
                    ans += num[j];        
            }
            else if(priority[j-1] == 1)
            {
                if(j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0)
                    ans -= num[j]/num[j+1];
                else if(priority[j+1] == 3 || priority[j+1] == 2)
                {
                    int p, tmp_ans = num[j]/num[j+1];
                    for(p=j+1; p<ind-1; p++)
                    {
                        if(priority[p] == 1 || priority[p] == 0)
                            break;
                        if(priority[p] == 3)
                            tmp_ans /= num[p+1];
                        else
                            tmp_ans *= num[p+1];
                    }

                    ans -= tmp_ans;
                    j = p;
                }         
                else
                    ans -= num[j];
            }
            else
            {
                if(j == ind-2)
                    ans = ans*num[j]/num[j+1];
                else
                    ans *= num[j];
            }
        }

        if(priority[j] == 2)
        {
            if(ind == 2)
                ans += num[j]*num[j+1];
            else if(j == 0)
            {
                if(priority[j+1] == 0 || priority[j+1] == 1)
                    ans += num[j]*num[j+1];
                else
                    ans += num[j];
            }
            else if(priority[j] == priority[j-1])
            {
                if(j == ind-2)
                    ans *= num[j]*num[j+1];
                else
                    ans *= num[j];
            }
            else if(priority[j-1] == 0)
            {
                if(j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0)
                    ans += num[j]*num[j+1];
                else if(priority[j+1] == 3 || priority[j+1] == 2)
                {
                    int p, tmp_ans = num[j]*num[j+1];
                    for(p=j+1; p<ind-1; p++)
                    {
                        if(priority[p] == 1 || priority[p] == 0)
                            break;
                        if(priority[p] == 3)
                            tmp_ans /= num[p+1];
                        else
                            tmp_ans *= num[p+1];
                    }

                    ans += tmp_ans;
                    j = p;
                }
                else
                    ans += num[j];
            }
            else if(priority[j-1] == 1)
            {
                if(j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0)
                    ans -= num[j]*num[j+1];
                else if(priority[j+1] == 3 || priority[j+1] == 2)
                {
                    int p, tmp_ans = num[j]*num[j+1];
                    for(p=j+1; p<ind-1; p++)
                    {
                        if(priority[p] == 1 || priority[p] == 0)
                            break;
                        if(priority[p] == 3)
                            tmp_ans /= num[p+1];
                        else
                            tmp_ans *= num[p+1];
                    }

                    ans -= tmp_ans;
                    j = p;
                }
                else
                    ans -= num[j];
            }
            else
            {
                if(j == ind-2)
                    ans = ans/num[j]*num[j+1];
                else
                    ans /= num[j];
            }
        }

        if(priority[j] == 1)
        {
            if(ind == 2)
                ans += num[j]-num[j+1];
            else if(j == 0)
                ans += num[j];
            else if(priority[j] == priority[j-1])
            {
                if(j == ind-2)
                    ans -= num[j]+num[j+1];
                else
                    ans -= num[j];
            }
            else if(priority[j-1] == 2 || priority[j-1] == 3)
            {
                if((j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0) && j != 1)
                    ans -= num[j+1];
            }
            else
            {
                if(j == ind-2)
                    ans = ans+num[j]-num[j+1];
                else
                    ans += num[j];
            }       
        }

        if(priority[j] == 0)
        {
            if(ind == 2)
                ans += num[j]+num[j+1];
            else if(j == 0)
                ans += num[j];
            else if(priority[j] == priority[j-1])
            {
                if(j == ind-2)
                    ans += num[j]+num[j+1];
                else
                    ans += num[j];
            }
            else if(priority[j-1] == 2 || priority[j-1] == 3)
            {
                if((j == ind-2 || priority[j+1] == 1 || priority[j+1] == 0) && j != 1)
                    ans += num[j+1];
            }
            else
            {
                if(j == ind-2)
                    ans = ans-num[j]+num[j+1];
                else
                    ans -= num[j];
            }    
        }
    }

    printf("\nans = %d\n", ans);
    display_7seg(fd, _7seg, ans);
    display_led(fd, ans);
    
    return ans;
}

int main(int argc, char *argv[])
{
    unsigned short key;
    unsigned short led;
    lcd_write_info_t lcd;
    _7seg_info_t _7seg;

    int fd, ret_key, ret_ans = -1;

    /* Open device /dev/lcd */
    if((fd = open("/dev/lcd", O_RDWR)) < 0)
    {
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }

    Clear_IO(fd, lcd, _7seg, key, led);
    memset(lcd.Msg, 0, sizeof(lcd.Msg));
    lcd.Count = 0;

    while(1)
    {
        ret_key = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        
        if(ret_key < 0)
        {
            sleep(1);
            continue;
        }
        else
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);

        ret_key = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        
        if((char)key == '#')   //#
        {
            ret_ans = calculate(fd, lcd, _7seg);
            key = 0x3d; //=
        }
        if((char)key == 'A') 
            key = 0x2b;   //+
        if((char)key == 'B')
            key = 0x2d;   //-
        if((char)key == 'C')
            key = 0x78;  //x
        if((char)key == 'D')
            key = 0x2f;   ///
        if((char)key == '*')
        {
            Clear_IO(fd, lcd, _7seg, key, led);
            memset(lcd.Msg, 0, sizeof(lcd.Msg));
            lcd.Count = 0;
        }
        else
        {
            if(ret_ans < 0)
            {
                lcd.Msg[lcd.Count] = key;
                lcd.Count++;
            }
            else
            {
                int k = 0;
                char ans_c[10];
                lcd.Msg[lcd.Count] = key;               //put '=' into Msg
                lcd.Count++;
                sprintf(ans_c, "%d", ret_ans);          //change ans's type from int to char*
                while(ans_c[k] != '\0')
                {
                    lcd.Msg[lcd.Count] = ans_c[k];
                    lcd.Count++;
                    k++;
                }
                lcd.Msg[lcd.Count] = '\n';              //change line and keep inputting
                lcd.Count++;
                last_row = lcd.Count;
                ret_ans = -1;
            }
            
            display_lcd(fd, lcd);
        }
    }

    close(fd);

    return 0;
}
