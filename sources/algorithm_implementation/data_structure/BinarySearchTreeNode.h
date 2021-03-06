#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <iostream>
#include <vector>
#include <stack>

namespace Mcoder
{

template <typename basetype>
class BinarySearchTreeNode
{
public:
    BinarySearchTreeNode<basetype> *parent;
    BinarySearchTreeNode<basetype> *lchild;
    BinarySearchTreeNode<basetype> *rchild;
    basetype data;

    BinarySearchTreeNode()
    {
        parent = nullptr;
        lchild = nullptr;
        rchild = nullptr;
    }
};

template <typename basetype>
class BinarySearchTree
{
public:
    BinarySearchTreeNode<basetype> *root;

    BinarySearchTree()
    {
        root = nullptr;
    }
    ~BinarySearchTree()
    {
        // 通过中序遍历来释放 node 的空间
        std::vector<BinarySearchTreeNode<basetype>*> nodes;

        // 中序遍历，迭代法
        if(root == nullptr) return;

        BinarySearchTreeNode<basetype> *curr=root;
        std::stack<BinarySearchTreeNode<basetype> *> st;

        while(!st.empty() || curr!=nullptr)
        {
            while(curr!=nullptr)
            {
                st.push(curr);
                curr=curr->lchild;
            }
            curr=st.top();
            st.pop();

            nodes.push_back(curr);
            curr=curr->rchild;
        }

        for (BinarySearchTreeNode<basetype>* node:nodes )
        {
            delete node;
        }

    }

    /**
     * @brief inorder_iterative
     * 迭代方式中序遍历
     * @return std::vector<basetype>
     */
    std::vector<basetype> inorderIterative()
    {
        return inorderIterative(root);
    }

    /**
     * @brief inorder_iterative
     * 递归方式中序遍历,实现算法
     * @param root 根节点
     * @return std::vector<basetype>
     */
    std::vector<basetype> inorderIterative(BinarySearchTreeNode<basetype> *root)
    {
        std::vector<basetype> ret;

        if(root == nullptr)
            return ret;

        // 中序遍历，迭代法
        BinarySearchTreeNode<basetype> *curr=root;
        std::stack<BinarySearchTreeNode<basetype> *> st;

        while(!st.empty() || curr!=nullptr)
        {
            while(curr!=nullptr)
            {
                st.push(curr);
                curr=curr->lchild;
            }
            curr=st.top();
            st.pop();

            ret.push_back(curr->data);
            curr=curr->rchild;
        }
        return ret;
    }

    /**
     * @brief search
     * 查找算法 递归
     * @param x
     * @param key
     * @return
     */
    BinarySearchTreeNode<basetype> *search(BinarySearchTreeNode<basetype> *x, const basetype& key)
    {
        if(x == nullptr || key == x->data)
            return x;
        if(key < x->data)
            return search(x->lchild, key);
        else
            return search(x->rchild, key);
    }

    /**
     * @brief searchIterative
     * 查找算法 迭代
     * @param key
     * @return
     */
    BinarySearchTreeNode<basetype> *searchIterative(const basetype& key)
    {
        BinarySearchTreeNode<basetype> *x = root;
        while(x!=nullptr && key != x->data)
        {
            if(key < x->data)
                x = x->lchild;
            else
                x = x->rchild;
        }
        return x;
    }

    /**
     * @brief minimum
     * 最小节点
     * @return
     */
    BinarySearchTreeNode<basetype> *minimum()
    {
        return minimum(root);
    }

    /**
     * @brief minimum
     * 最小节点
     * @param node
     * @return
     */
    BinarySearchTreeNode<basetype> *minimum(BinarySearchTreeNode<basetype> *node)
    {
        if(node == nullptr)
            return nullptr;
        while(node->lchild != nullptr)
            node = node->lchild;
        return node;
    }

    /**
     * @brief maximum
     * 最大节点
     * @return
     */
    BinarySearchTreeNode<basetype> *maximum()
    {
        return maximum(root);
    }

    BinarySearchTreeNode<basetype> *maximum(BinarySearchTreeNode<basetype> *node)
    {
        if(node == nullptr)
            return nullptr;
        while (node->rchild != nullptr)
            node = node->rchild;
        return node;
    }

    /**
     * @brief successor
     * 查找后继节点
     * @param node
     * @return
     */
    BinarySearchTreeNode<basetype> *successor(BinarySearchTreeNode<basetype> *node)
    {
        // 右支的最小点
        if(node->rchild != nullptr)
            return minimum(node->rchild);

        // 如果节点没有右支，往上层找
        BinarySearchTreeNode<basetype> *y = node->parent;
        BinarySearchTreeNode<basetype> *x = node;

        while(y != nullptr && x == y->rchild)
        {
            x=y;
            y = y->parent;
        }
        return y;
    }

    /**
     * @brief predecessor
     * 前一个节点
     * @param node
     * @return
     */
    BinarySearchTreeNode<basetype> *predecessor(BinarySearchTreeNode<basetype> *node)
    {
        // 左支的最大值
        if(node->lchild != nullptr)
            return maximum(node->lchild);

        BinarySearchTreeNode<basetype> *y = node->parent;
        BinarySearchTreeNode<basetype> *x = node;
        while (y != nullptr && x == y->lchild)
        {
            // x = y.lchild means that x < y
            // x = y.rchild means that x > y
            x = y;
            y = y->parent;
        }

        return y;
    }

    /**
     * @brief insert
     * 插入算法
     * @param value
     * @return
     */
    BinarySearchTreeNode<basetype> *insert(basetype value)
    {
        BinarySearchTreeNode<basetype> *node = new BinarySearchTreeNode<basetype>;
        node->data = value;

        return insert(node);
    }

    BinarySearchTreeNode<basetype> *insert(BinarySearchTreeNode<basetype> *node)
    {
        BinarySearchTreeNode<basetype> *y = nullptr;
        BinarySearchTreeNode<basetype> *x = root;
        while(x != nullptr)
        {
            y = x;
            if(node->data < y->data)
                x = x->lchild;
            else
                x = x->rchild;
        }

        node->parent = y;
        if(y == nullptr)
            root = node;
        else if(node->data < y->data)
            y->lchild = node;
        else
            y->rchild = node;

        return node;
    }

    /**
     * @brief transplant
     * 移植
     * @param u
     * @param v
     */
    void transplant(BinarySearchTreeNode<basetype> *u, BinarySearchTreeNode<basetype> *v)
    {
        if(u->parent == nullptr)
            root = v;
        else if(u == u->parent->lchild)
            u->parent->lchild = v;
        else
            u->parent->rchild = v;

        if(v != nullptr)
            v->parent = u->parent;
    }

    /**
     * @brief remove
     * 删除算法
     * @param z
     */
    void remove(BinarySearchTreeNode<basetype> *z)
    {
        if(z->lchild == nullptr)
            transplant(z, z->rchild);
        else if(z->rchild == nullptr)
            transplant(z, z->lchild);
        else
        {
            BinarySearchTreeNode<basetype>* y = minimum(z->rchild);
            if(y->parent != z)
            {
                transplant(y, y->rchild);
                y->rchild = z->rchild;
                y->rchild->parent = y;
            }

            transplant(z, y);
            y->lchild = z->lchild;
            y->lchild->parent = y;
        }
    }

};

}

#endif // BINARY_SEARCH_TREE_H
