#include "cdll.h"

#include <stdexcept>

// Initialise single item, with next/prev pointing to ourself
cdll::Node::Node(int keyValue)
:   key(keyValue)
,   next(this)
,   prev(this)
{
}

cdll::Node* cdll::find(Node* cdll, int key)
{
    if (cdll == nullptr)
    {
        return nullptr;
    }

    Node* node = cdll;
    do
    {
        if (node->key == key)
        {
            return node;
        }
        node = node->next;
    } while (node != cdll);

    return nullptr;
}

// Insert before the given node
void cdll::insert(cdll::Node** cdll, int key)
{
    if (cdll == nullptr)
    {
        throw std::runtime_error("invalid argument");
    }

    Node* head = *cdll;

    Node* newNode = new Node(key);

    // Check for empty structure
    if (head)
    {
        Node* old_previous = head->prev;
        newNode->prev = old_previous; // Must update the new nodes prev pointer 
        newNode->next = head;
        old_previous->next = newNode;
        head->prev = newNode;
    }

    *cdll = newNode;
}

void cdll::remove(cdll::Node** cdll, int keyValue)
{
    if (cdll == nullptr)
    {
        throw std::runtime_error("invalid argument");
    }

    Node* nodeToRemove = find(*cdll, keyValue);
    if (nodeToRemove == nullptr)
    {
        // Not found
        throw std::runtime_error("key not present in list");
    }

    // Still works for 1 node. This avoids having multiple return points in this function
    nodeToRemove->prev->next = nodeToRemove->next;
    nodeToRemove->next->prev = nodeToRemove->prev;

    // If we are removing the last item, we need to clear the head pointer
    if (nodeToRemove->next == nodeToRemove)
    {
        // No need to check prev, it will always be nodeToRemove
        *cdll = nullptr;
    }
    else if (*cdll == nodeToRemove)
    {
        // if we're removing the head, increment the head pointer
        *cdll = nodeToRemove->next;
    }

    delete nodeToRemove;
}

