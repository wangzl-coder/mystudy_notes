#include <stdio.h>
#include <stdlib.h>
#include "list.h"


static const char* n_weather[] = {"sunny", "cloudy", "overcast", "light rain", "moderate rain"};

struct weather_st{
    int year;
    int month;
    int day;
    const char *weather;
};

int main()
{
    int ret = 0;
    int i = 0;
    list_head *wth_head = NULL;
    list_node *curr_node = NULL;
    struct weather_st wth;

    wth_head = list_head_init(sizeof(struct weather_st));
    if(wth_head == NULL) {
        fprintf(stderr, "student head init failed \r\n");
        exit(-1);
    }
    wth.year = 2022;
    wth.month = 10;
    for(i = 31; i > 0; i--) {
        wth.day = i;
        wth.weather = n_weather[i%5];
        ret = list_insert_head(wth_head, &wth);
        if(ret < 0) {
            goto list_release;
        }
        //list_insert_tail(wth_head, &wth);
    }
    ret = list_delete_tail(wth_head);
    ret = list_delete_head(wth_head);
    list_for_each(wth_head, curr_node) {
        struct weather_st *curr_wth = data_for_node(curr_node);
        if(curr_wth->day == 20){
            curr_wth->weather = "new weather";
            ret = list_update_node(wth_head, curr_node, curr_wth);
            if(ret < 0)
                fprintf(stderr, "update weather failed \r\n");
            break;
        }
    }
    list_for_each(wth_head, curr_node) {
        struct weather_st *curr_wth = data_for_node(curr_node);
        char wstr[50];
        snprintf(wstr, 50, "%d-%d-%d: %s", curr_wth->year, curr_wth->month, curr_wth->day, curr_wth->weather);
        puts(wstr);
    }

list_release:
    list_release(wth_head);
    exit(ret);
}
