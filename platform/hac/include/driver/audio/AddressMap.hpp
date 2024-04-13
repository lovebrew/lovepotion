#pragma once

#include "driver/audio/MemoryPool.hpp"
#include "driver/audio/RedBlackTree.hpp"

static RedBlackTree addressMap;

namespace love
{
    struct AddressMapNode
    {
        RedBlackNode node;
        Chunk chunk;
    };

#define getAddressMapNode(x) rbtree_item((x), AddressMapNode, node)

    static int addressNodeComparator(const RedBlackNode* left, const RedBlackNode* right)
    {
        const auto* _left  = getAddressMapNode(left)->chunk.address;
        const auto* _right = getAddressMapNode(right)->chunk.address;

        if (_left < _right)
            return -1;

        if (_left > _right)
            return 1;

        return 0;
    }

    static void addressMapNodeDestructor(RedBlackNode* node)
    {
        free(getAddressMapNode(node));
    }

    static AddressMapNode* getNode(void* address)
    {
        AddressMapNode node {};

        node.chunk.address = (uint8_t*)address;
        auto* result       = addressMap.find(&node.node);

        return result ? getAddressMapNode(result) : nullptr;
    }

    static AddressMapNode* newNode(const Chunk& chunk)
    {
        auto* node = (AddressMapNode*)malloc(sizeof(AddressMapNode));

        if (node == nullptr)
            return nullptr;

        node->chunk = chunk;

        return node;
    }

    static void deleteNode(AddressMapNode* node)
    {
        addressMap.remove(&node->node, addressMapNodeDestructor);
    }
} // namespace love
