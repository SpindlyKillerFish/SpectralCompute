#ifndef SPECTRAL_CODING_TEST_CDLL_H
#define SPECTRAL_CODING_TEST_CDLL_H

/**
 *  This is a (currently buggy!) implementation of a circular doubly-linked list.
 *
 *  It is written using an extremely traditional C-like dialect of C++, of the kind that could be found in legacy
 *  codebases, but wouldn't be desirable in new projects.
 *
 *  The implementation contains bugs, and there are many ways in which it can be improved. The task is to find and fix
 *  (or suggest fixes) to the bugs, and write unit tests for the circular doubly-linked list. This mimics the first
 *  steps of starting to work on a legacy code base.
 */
namespace cdll
{

struct Node
{
    Node(int key);
    int key;
    Node* next;
    Node* prev;
};

Node* find(Node* cdll, int key);

void insert(Node** cdll, int key);

void remove(Node** cdll, int key);

}

#endif //SPECTRAL_CODING_TEST_CDLL_H
