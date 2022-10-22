#include <stdio.h>
#include <stdlib.h>
#include "weather_list.h"



static const char* n_weather[] = {"sunny", "cloudy", "overcast", "light rain", "moderate rain"};


int main()
{
    int ret, i;
    wthlist *wth_head = NULL;
    struct wthinfo_st weather_info;
    struct weather_date date;

    ret = weather_list_create(&wth_head);
    if(ret < 0) {
        puts("weather head create failed");
        exit(ret);
    }
    weather_info.date.year = 2022;
    weather_info.date.month = 8;
    for(i = 31; i > 0; i--) {
        weather_info.date.day = i;
        weather_info.weather = n_weather[i%5];
        ret = weather_list_add_head(wth_head, &weather_info);
        if(ret < 0) {
            puts("weather info add failed");
            break;
        }
    }
    weather_list_delete_head(wth_head);
    weather_list_delete_tail(wth_head);
    date.year = 2022;
    date.month = 8;
    date.day = 6;
    weather_list_delete_date(wth_head, &date);
    weather_info.date.year = 2022;
    weather_info.date.month = 8;
    weather_info.date.day = 6;
    weather_info.weather = "my_weather";
    weather_list_add_order(wth_head, &weather_info);
    weather_list_display(wth_head);
    date.day = 20;
    printf("weather of 2022-8-20 is %s \r\n", weather_list_query(wth_head, &date));
    puts("update!");
    weather_info.date.year = 2022;
    weather_info.date.month = 8;
    weather_info.date.day = 20;
    weather_info.weather = "heavy rain";
    weather_list_update(wth_head, &weather_info);
    printf("weather of 2022-8-20 is %s \r\n", weather_list_query(wth_head, &date));

    weather_list_release(&wth_head);
    exit(ret);
}
