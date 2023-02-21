#if 0
    二叉树：每个节点最多只有两个出边的数：一个树，其节点具有0个，1个，2个子节点

        1 二叉搜索树(BST):
            特点：
                .根的左分支节点值都小于根节点值
                .右分支节点值都大于根节点值
                .所有的子树也都是二叉搜索树
            
        2 自平衡二叉搜索树：
            节点深度：从其根节点，到达它一共需要经过的父节点数目
            叶子节点：没有子节点的处于树底层的节点
            树的高度：树中的处于最底层节点的深度
            平衡二叉搜索树：所有叶子节点的深度相差不超过1的二叉搜索树
            自平衡二叉搜索树：操作都试图维持平衡的二叉搜索树

            （1）红黑树：一种自平衡二叉搜索树
                a. 所有的节点要么着红色，要么着黑色
                b. 叶子节点都是黑色
                c. 叶子节点不包含数据
                d. 所有的非叶子节点都有两个子节点
                e. 如果一个节点是红色，则他的子节点都是黑色
                f. 在一个节点到其叶子节点的路径上，如果总是包含同样数目的黑色节点，则该路径相比其他路径最短

                上述条件保证了最深的叶子节点的深度不会大于两倍的最浅叶子节点的深度，所以红黑树是半平衡的。

            （2）rbtree
                Linux实现的红黑树称为rbtree，根节点描述：
                struct rb_root root = RB_ROOT;
                数的其他节点由结构rb_node描述
                rbtree的实现并没有提供搜索和插入例程，这些例程希望由rbtree的用户自己定义
#endif
/* Ex: */

/*
 * 搜索例程：在页高速缓存中搜索一个文件区（由一个i节点和一个偏移量共同描述），
 *      每个i节点都有自己的rbtree, 以惯量在文件中的页偏移，该函数将搜索给定i
 *      节点的rbtree,以寻找匹配的偏移值
 * */
static page *rb_search_page_cache(struct inode *inode, unsigned long oggset)
{
    struct rb_node *n = inode->i_rb_page_cache.rb_node;

    while(n) {
        struct page *page = rb_entry(n, struct page, rb_page_cache);
        if(offset < page->offset)
            n = n->rb_left;
        else if(offset > page->offset)
            n = n->rb->right;
        else
            return page;
    }
    return NULL;
}

/*
 * 插入操作
 *
 * */
struct page *rb_insert_page_cache(struct inode *inode, unsigned long offset, struct rb_node *node)
{
    struct rb_node **p = &inode->i_rb_page_cache.rb_node;
    struct rb_node *parent = NULL;
    struct page *page;

    while(*p) {
        parent = *p;
        page = rb_entry(parent, struct page, rb_page_cache);

        if(offset < page->offset)
            p = &(*p)->rb_left;
        else if(offset > page->offset)
            p = &(*page)->rb_right;
        else
            return page;
    }
    rb_link_node(node, parent, p);
    rb_insert_color(node, &inode->i_rb_page_cache);
    return NULL;
}
