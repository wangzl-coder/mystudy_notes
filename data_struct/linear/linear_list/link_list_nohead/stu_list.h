#ifndef STU_LIST_H__
#define STU_LIST_H__


struct student_st
{
    int id;
    char *name;
    unsigned char age;
    char *gender;
    unsigned char class;
};


struct stu_node
{
    struct student_st stu_info;
    struct stu_node *next;
};

int student_insert_head(struct stu_node**, struct student_st*);

int student_insert_tail(struct stu_node**, struct student_st*);

void student_info_display(struct stu_node*);

int student_delete_head(struct stu_node**);

int student_delete_tail(struct stu_node**);

int student_query_byid(struct stu_node*, struct student_st*);

int student_update_info(struct stu_node*, struct student_st);

void students_release(struct stu_node**);

#endif
