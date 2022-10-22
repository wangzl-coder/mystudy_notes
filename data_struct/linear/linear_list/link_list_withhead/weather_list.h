#ifndef HEAD_LINK_LIST_H__
#define HEAD_LINK_LIST_H__


#define WEATHER_EARLIEST_YEAR 2000

typedef struct weather_date{
    int year;
    int month;
    int day;
}wth_date;

struct wthinfo_st{
    wth_date date;
    const char *weather;
};


typedef struct wthlist_st{
    struct wthinfo_st wthinfo;
    struct wthlist_st *next;
}wthlist;


int weather_list_create(wthlist **wth_head);

wthlist * weather_list_create1(void);

int weather_list_add_head(wthlist *wth_head, struct wthinfo_st *pwtinfo);

int weather_list_add_tail(wthlist *wth_head, struct wthinfo_st *pwtinfo);

int weather_list_add_order(wthlist *wth_head, struct wthinfo_st *pwtinfo);

int weather_list_delete_head(wthlist *wth_head);

int weather_list_delete_tail(wthlist *wth_head);

int weather_list_delete_date(wthlist *wth_head, wth_date *date);

const char* weather_list_query(wthlist *wth_head, wth_date *date);

void weather_list_display(wthlist *wth_head);

int weather_list_update(wthlist *wth_head, struct wthinfo_st *pwtinfo);

void weather_list_release(wthlist **wth_head);





#endif
