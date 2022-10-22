#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "weather_list.h"


static int date_compare(wth_date date1, wth_date date2)
{
    if(date1.year != date2.year)
        return (date1.year - date2.year);

    if(date1.month != date2.month)
        return (date1.month - date2.month);

    if(date1.day != date2.day)
        return (date1.day - date2.day);

    return 0;
}

static bool date_isvalid(wth_date *date)
{
    int max_day = 0;
    if(date->year < WEATHER_EARLIEST_YEAR || date->day < 1)
        return false;

    if(date->month < 1 || date->month > 12)
        return false;

    if(date->month != 2) {
        max_day = 30 + ((date->month < 8) ^ (date->month%2 == 0)); /* 1-7: 奇数月大月, 8-12: 偶数月大月 */
    } else {
        max_day = (date->year%4 == 0)? 29 : 28;
    }
    return (date->day <= max_day);
}

int weather_list_create(wthlist **wth_head)
{
    if(wth_head == NULL)
        return -EINVAL;
    
    *wth_head = malloc(sizeof(struct wthlist_st));
    if(*wth_head == NULL)
        return -ENOMEM;
    
    (*wth_head)->next = NULL;

    return 0;
}

wthlist * weather_list_create1()
{
    struct wthlist_st *tmp = NULL;

    tmp = malloc(sizeof(struct wthlist_st));
    if(tmp)
        tmp->next = NULL;
    return tmp;
}

int weather_list_add_head(wthlist *wth_head, struct wthinfo_st *pwtinfo)
{
    struct wthlist_st *wth_tmp = NULL;

    if(wth_head == NULL || pwtinfo == NULL)
        return -EINVAL;
    
    if(!date_isvalid(&pwtinfo->date))
        return -EINVAL;

    wth_tmp = malloc(sizeof(struct wthlist_st));
    if(!wth_tmp)
        return -ENOMEM;

    wth_tmp->wthinfo = *pwtinfo;
    wth_tmp->next = wth_head->next;
    wth_head->next = wth_tmp;
    return 0;
}

int weather_list_add_tail(wthlist *wth_head, struct wthinfo_st *pwtinfo)
{
    struct wthlist_st *wth_last = wth_head;
    struct wthlist_st *wth_tmp = NULL;
    
    if(wth_head == NULL || pwtinfo == NULL)
        return -EINVAL;

    if(!date_isvalid(& pwtinfo->date))
        return -EINVAL;
    
    while(wth_last->next != NULL) {
        wth_last = wth_last->next;
    }
    wth_tmp = malloc(sizeof(struct wthlist_st));
    if(wth_tmp == NULL)
        return -ENOMEM;

    wth_tmp->wthinfo = *pwtinfo;
    wth_tmp->next = NULL;
    wth_last->next = wth_tmp;
    return 0;
}

int weather_list_add_order(wthlist *wth_head, struct wthinfo_st *pwtinfo)
{
    struct wthlist_st *tmp, *wth_next, *inswealist;
    if(wth_head == NULL || pwtinfo == NULL)
        return -EINVAL;
    
    if(!date_isvalid(&pwtinfo->date))
        return -EINVAL;

    tmp = wth_head;
    while(tmp->next != NULL) {
        wth_next = tmp->next;
        if(date_compare(wth_next->wthinfo.date, pwtinfo->date) >= 0)
            break;
        tmp = wth_next;
    }
    inswealist = malloc(sizeof(struct wthlist_st));
    if(inswealist == NULL)
        return -ENOMEM;

    inswealist->wthinfo = *pwtinfo;
    inswealist->next = wth_next;
    tmp->next =  inswealist;
    return 0;
}

int weather_list_delete_head(wthlist *wth_head)
{
    struct wthlist_st *tmp = NULL;
    if(wth_head == NULL)
        return -EINVAL;

    tmp = wth_head->next;
    wth_head->next = tmp->next;
    if(tmp != NULL)
        free(tmp);
    return 0;
}

int weather_list_delete_tail(wthlist *wth_head)
{
    struct wthlist_st *last = wth_head;
    struct wthlist_st *pre = NULL;
    if(wth_head == NULL)
        return -EINVAL;
    
    while(last->next != NULL) {
        pre = last;
        last = last->next;
    }
    if(pre != NULL) {
        pre->next = NULL;
        free(last);
    }
    return 0;
}

int weather_list_delete_date(wthlist *wth_head, wth_date *date)
{
    struct wthlist_st *tmp = wth_head;
    struct wthlist_st *pre = NULL;

    if(wth_head == NULL || date == NULL)
        return -EINVAL;
    
    if(!date_isvalid(date))
        return -EINVAL;
    
    while(1) {
        pre = tmp;
        tmp = tmp->next;
        if(tmp == NULL)
            break;
        else if(date_compare(tmp->wthinfo.date, *date) == 0)
            break;
    }
    if(tmp != NULL) {
        pre->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
        return 0;
    } else
        return -EINVAL;
}

const char* weather_list_query(wthlist *wth_head, wth_date *date)
{
    struct wthlist_st *tmp = NULL;

    if(wth_head == NULL || date == NULL)
        return NULL;
    if(!date_isvalid(date))
        return NULL;

    tmp = wth_head->next;
    while(tmp != NULL) {
        if(date_compare(tmp->wthinfo.date, *date) == 0)
            break;
        tmp = tmp->next;
    }
    if(tmp == NULL)
        return NULL;
    return tmp->wthinfo.weather;
}

int weather_list_update(wthlist *wth_head, struct wthinfo_st *pwtinfo)
{
    struct wthlist_st *tmp = NULL;

    if(wth_head == NULL || pwtinfo == NULL)
        return -EINVAL;

    if(!date_isvalid(& pwtinfo->date))
        return -EINVAL;
    
    tmp = wth_head->next;
    while(tmp != NULL) {
        if(date_compare(tmp->wthinfo.date, pwtinfo->date) == 0)
            break;
        tmp = tmp->next;
    }
    if(tmp == NULL)
        return -EINVAL;

    tmp->wthinfo.weather = pwtinfo->weather;
    return 0;
}

void weather_list_display(wthlist *wth_head)
{
    struct wthlist_st *wth_tmp = wth_head;
    if(wth_head == NULL)
        return ;
    
    while(wth_tmp->next != NULL) {
        wth_tmp = wth_tmp->next;
        printf("%d-%d-%d: %s \r\n", wth_tmp->wthinfo.date.year, wth_tmp->wthinfo.date.month, 
                wth_tmp->wthinfo.date.day, wth_tmp->wthinfo.weather);
    }
}

void weather_list_release(wthlist **wth_head)
{
    struct wthlist_st *wth_next = NULL;

    if(wth_head == NULL)
        return ;

    while((*wth_head) != NULL) {
        wth_next = (*wth_head)->next;
        (*wth_head)->next = NULL;
        free(*wth_head);
        *wth_head = wth_next;
    }
}
