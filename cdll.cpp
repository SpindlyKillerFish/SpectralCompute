#include "cdll.h"

#include <stdexcept>

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

void cdll::insert(cdll::Node** cdll, int key)
{
    if (cdll == nullptr)
    {
        throw std::runtime_error("invalid argument");
    }

    Node* head = *cdll;

    Node* newNode = new Node();
    newNode->key = key;
    newNode->prev = newNode;
    newNode->next = newNode;

    if (head == nullptr)
    {
        *cdll = newNode;
        return;
    }

    newNode->next = head;
    head->prev->next = newNode;
    head->prev = newNode;

    *cdll = newNode;
}

void cdll::remove(cdll::Node** cdll, int key)
{
    Node* nodeToRemove = cdll::find(*cdll, key);

    if (nodeToRemove == nullptr)
    {
        throw std::runtime_error("key not present in list");
    }

    if (nodeToRemove->next == nodeToRemove)
    {
        // only node in list
        *cdll = nullptr;
        delete nodeToRemove;
        return;
    }

    nodeToRemove->prev->next = nodeToRemove->next;
    nodeToRemove->next->prev = nodeToRemove->prev;

    if (*cdll == nodeToRemove)
    {
        // if we're removing the head, increment the head pointer
        *cdll = nodeToRemove->next;
    }

    delete nodeToRemove;
}
