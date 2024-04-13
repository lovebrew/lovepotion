#pragma once

#include <stddef.h>
#include <stdint.h>

#define rbtree_item(ptr, type, member) ((type*)(((char*)ptr) - offsetof(type, member)))

struct RedBlackNode;
typedef int (*RedBlackTreeComparator)(const RedBlackNode* left, const RedBlackNode* right);
typedef void (*RedBlackNodeDestructor)(RedBlackNode* node);

struct RedBlackNode
{
    uintptr_t parentColor;
    RedBlackNode* child[2];
};

class RedBlackTree
{
  public:
    RedBlackNode* root;
    RedBlackTreeComparator comparator;
    size_t _size;

    void init(RedBlackTreeComparator comparator)
    {
        this->root       = nullptr;
        this->comparator = comparator;
        this->_size      = 0;
    }

    int empty()
    {
        return this->root == nullptr;
    }

    size_t size()
    {
        return this->_size;
    }

    enum Color
    {
        RED,
        BLACK
    };

    static constexpr auto COLOR_MASK = (RED | BLACK);

    RedBlackNode* find(const RedBlackNode* node)
    {
        RedBlackNode* temp = this->root;
        RedBlackNode* save = nullptr;

        while (temp != nullptr)
        {
            int result = (*this->comparator)(node, temp);

            if (result < 0)
                temp = temp->child[LEFT];
            else if (result > 0)
                temp = temp->child[RIGHT];
            else
            {
                save = temp;
                temp = temp->child[LEFT];
            }
        }

        return save;
    }

    RedBlackNode* min()
    {
        RedBlackNode* node = doMinMax(this, LEFT);
        return node;
    }

    RedBlackNode* max()
    {
        RedBlackNode* node = doMinMax(this, RIGHT);
        return node;
    }

    RedBlackNode* insert(RedBlackNode* node)
    {
        return doInsert(this, node, 0);
    }

    RedBlackNode* insertMultiple(RedBlackNode* node)
    {
        return doInsert(this, node, 1);
    }

    RedBlackNode* next(const RedBlackNode* node)
    {
        return doIterate(node, RIGHT);
    }

    RedBlackNode* prev(const RedBlackNode* node)
    {
        return doIterate(node, LEFT);
    }

    static void rotate(RedBlackTree* tree, RedBlackNode* node, int left)
    {
        RedBlackNode* temp   = node->child[left];
        RedBlackNode* parent = getParent(node);

        node->child[left] = temp->child[!left];
        if (temp->child[!left] != nullptr)
            setParent(temp->child[!left], node);

        temp->child[!left] = node;
        setParent(temp, parent);

        if (parent != nullptr)
        {
            if (node == parent->child[!left])
                parent->child[!left] = temp;
            else
                parent->child[left] = temp;
        }
        else
            tree->root = temp;

        setParent(node, temp);
    }

    RedBlackNode* remove(RedBlackNode* node, RedBlackNodeDestructor destructor)
    {
        Color color;
        RedBlackNode* parent;
        RedBlackNode* child;
        RedBlackNode* original = node;
        RedBlackNode* next;

        next = this->next(node);

        if (node->child[LEFT] != nullptr && node->child[RIGHT] != nullptr)
        {
            RedBlackNode* old = node;
            node              = node->child[RIGHT];

            while (node->child[LEFT] != nullptr)
                node = node->child[LEFT];

            parent = getParent(old);

            if (parent != nullptr)
            {
                if (parent->child[LEFT] == old)
                    parent->child[LEFT] = node;
                else
                    parent->child[RIGHT] = node;
            }
            else
                this->root = node;

            child  = node->child[RIGHT];
            parent = getParent(node);
            color  = getColor(node);

            if (parent == old)
                parent = node;
            else
            {
                if (child != nullptr)
                    setParent(child, parent);

                parent->child[LEFT] = child;
                node->child[RIGHT]  = old->child[RIGHT];
                setParent(old->child[RIGHT], node);
            }

            node->parentColor = old->parentColor;
            node->child[LEFT] = old->child[LEFT];
            setParent(old->child[LEFT], node);
        }
        else
        {
            if (node->child[LEFT] == nullptr)
                child = node->child[RIGHT];
            else
                child = node->child[LEFT];

            parent = getParent(node);
            color  = getColor(node);

            if (child != nullptr)
                setParent(child, parent);

            if (parent != nullptr)
            {
                if (parent->child[LEFT] == node)
                    parent->child[LEFT] = child;
                else
                    parent->child[RIGHT] = child;
            }
            else
                this->root = child;
        }

        if (color == BLACK)
            reColor(this, parent, child);

        if (destructor)
            (*destructor)(original);

        this->_size--;

        return next;
    }

    void clear(RedBlackNodeDestructor destructor)
    {
        RedBlackNode* node = this->root;

        while (this->root != nullptr)
        {
            while (node->child[LEFT] != nullptr)
                node = node->child[LEFT];

            if (node->child[RIGHT] != nullptr)
                node = node->child[RIGHT];
            else
            {
                RedBlackNode* parent = getParent(node);

                if (parent == nullptr)
                    this->root = nullptr;
                else
                    parent->child[node != parent->child[LEFT]] = nullptr;

                if (destructor)
                    (*destructor)(node);

                node = parent;
            }
        }

        this->_size = 0;
    }

  private:
    static constexpr uint8_t LEFT  = 0;
    static constexpr uint8_t RIGHT = 1;

    static void setBlack(RedBlackNode* node)
    {
        node->parentColor &= ~COLOR_MASK;
        node->parentColor |= BLACK;
    }

    static void setRed(RedBlackNode* node)
    {
        node->parentColor &= ~COLOR_MASK;
        node->parentColor |= RED;
    }

    static Color getColor(const RedBlackNode* node)
    {
        if (node == nullptr)
            return BLACK;

        return (Color)(node->parentColor & COLOR_MASK);
    }

    static int isBlack(const RedBlackNode* node)
    {
        return getColor(node) == BLACK;
    }

    static int isRed(const RedBlackNode* node)
    {
        return getColor(node) == RED;
    }

    static RedBlackNode* getParent(const RedBlackNode* node)
    {
        return (RedBlackNode*)(node->parentColor & ~COLOR_MASK);
    }

    static void setParent(RedBlackNode* node, const RedBlackNode* parent)
    {
        node->parentColor = (getColor(node) | ((uintptr_t)parent));
    }

    static RedBlackNode* doInsert(RedBlackTree* tree, RedBlackNode* node, int multiple)
    {
        RedBlackNode* original = node;
        RedBlackNode** temp    = &tree->root;
        RedBlackNode* parent   = nullptr;
        RedBlackNode* save     = nullptr;

        while (*temp)
        {
            int compare = (*tree->comparator)(node, *temp);
            parent      = *temp;

            if (compare < 0)
                temp = &((*temp)->child[LEFT]);
            else if (compare > 0)
                temp = &((*temp)->child[RIGHT]);
            else
            {
                if (!multiple)
                    save = *temp;

                temp = &((*temp)->child[LEFT]);
            }
        }

        if (save)
            return save;

        *temp = node;

        node->child[LEFT] = node->child[RIGHT] = nullptr;
        setParent(node, parent);

        setRed(node);

        while (isRed((parent = getParent(node))))
        {
            RedBlackNode* grandparent = getParent(parent);
            int left                  = (parent == grandparent->child[LEFT]);
            RedBlackNode* uncle       = grandparent->child[left];

            if (isRed(uncle))
            {
                setBlack(uncle);
                setBlack(parent);
                setRed(grandparent);

                node = grandparent;
            }
            else
            {
                if (parent->child[left] == node)
                {
                    RedBlackNode* temp;

                    rotate(tree, parent, left);

                    temp   = parent;
                    parent = node;
                    node   = temp;
                }

                setBlack(parent);
                setRed(grandparent);
                rotate(tree, grandparent, !left);
            }
        }

        setBlack(tree->root);
        tree->_size++;

        return original;
    }

    static RedBlackNode* doIterate(const RedBlackNode* node, int next)
    {
        RedBlackNode* iterator = (RedBlackNode*)node;

        if (iterator->child[next] != nullptr)
        {
            iterator = iterator->child[next];

            while (iterator->child[!next] != nullptr)
                iterator = iterator->child[!next];
        }
        else
        {
            RedBlackNode* parent = getParent(node);

            while (parent != nullptr && iterator == parent->child[next])
            {
                iterator = parent;
                parent   = getParent(iterator);
            }

            iterator = parent;
        }

        return iterator;
    }

    static RedBlackNode* doMinMax(const RedBlackTree* tree, int max)
    {
        RedBlackNode* node = tree->root;

        if (node == nullptr)
            return nullptr;

        while (node->child[max] != nullptr)
            node = node->child[max];

        return node;
    }

    static void reColor(RedBlackTree* tree, RedBlackNode* parent, RedBlackNode* node)
    {
        RedBlackNode* sibling = nullptr;

        while (isBlack(node) && node != tree->root)
        {
            int left = (node == parent->child[LEFT]);
            sibling  = parent->child[left];

            if (isRed(sibling))
            {
                setBlack(sibling);
                setRed(parent);
                rotate(tree, parent, left);

                sibling = parent->child[left];
            }

            if (isBlack(sibling->child[LEFT]) && isBlack(sibling->child[RIGHT]))
            {
                setRed(sibling);
                node   = parent;
                parent = getParent(node);
            }
            else
            {
                if (isBlack(sibling->child[left]))
                {
                    setBlack(sibling->child[!left]);
                    setRed(sibling);
                    rotate(tree, sibling, !left);

                    sibling = parent->child[left];
                }

                if (isBlack(parent))
                    setBlack(sibling);
                else
                    setRed(sibling);

                setBlack(parent);
                setBlack(sibling->child[left]);

                rotate(tree, parent, left);

                node = tree->root;
            }
        }

        if (node != nullptr)
            setBlack(node);
    }
};
