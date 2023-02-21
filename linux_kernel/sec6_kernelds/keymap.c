#if 0
    映射：通常也称为关联数组，键值对,基本三个操作：
        Add (key, value)
        Remove (key)
        value = Lookup(key)
        实现方式：散列表(线性复杂度)和二叉树(对数复杂度)
        Linux idr映射：映射一个唯一的标识数UID到一个指针，idr数据结构用于映射用户空间的的UID

        1 初始化一个idr
            void idr_init(struct idr *idp);

        2 分配一个新的UID
            （1）调整后备树大小：
                int idr_pre_get(struct idr *idp, gfp_t gfp_mask);
            （2）实际执行获取新的UID，并且将其加到idr；
                int idr_get_new(struct idr *idp, void *ptr, int *id)
                使用idp所指向的idr去分配一个新的UID，并且将其关联到指针ptr上，并将新的UID存于id
                @return: o for success, or -EAGAIN(try again), -ENOSPEC(idr is full);
ex:
int id;
do {
    if(!idr_ptr_get(&idr_hub, GFP_KERNEL))
        return -NOSPEC;
    ret = idr_get_new(&idr_huh, ptr, &id);
} while(ret == -EAGAIN)
                
            int idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id);
            确保新的UID大于或等于starting_id,允许idr的使用者确保UID不会被重用，允许其值
            不但在当前分配的ID中唯一，而且还保证在系统的整个运行期间唯一：
ex:

int id;
do {
    if(!idr_pre_get(&idr_huh, GFP_KERNEL))
        return -ENOSPEC;
    ret = idr_get_new_above(&idr_huh, ptr, next_id, &id);
} while(ret == -EAGAIN)

if(!ret)
    next_id = id + 1;

        3 查找UID：
            void *idr_find(struct idr *idp, int id);

        4 删除UID
            从idr中删除UID：
            void idr_remove(struct idr *idp, int id);

        5 撤销idr:
            void idr_destroy(struct idr *idp);
            注意：只能释放idr中未使用的内存，并不释放当前分配给UID使用的任何内存。
#endif
