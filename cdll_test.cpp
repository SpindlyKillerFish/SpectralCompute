#include "gtest/gtest.h"
#include "cdll.h"
#include <stdexcept>
#include <vector>
#include <list>

// Check every node's neighbours point back at it, and the list is circular
void CheckConnections(cdll::Node* head)
{
    if (not head)
    {
        return;
    }

    cdll::Node* node = head;
    do
    {
        // Each node must have a next and prv
        ASSERT_NE(node->next, nullptr);
        ASSERT_NE(node->prev, nullptr);

        EXPECT_EQ(node->next->prev, node) << "next->prev broken at key " << node->key;
        EXPECT_EQ(node->prev->next, node) << "prev->next broken at key " << node->key;

        node = node->next;
    } while (node != head);
}

void DestroyTestList(cdll::Node** head)
{
    if (not *head)
    {
        return;
    }

    cdll::Node* node = (*head)->next;
    while (node != *head)
    {
        cdll::Node* next = node->next;
        delete node;
        node = next;
    }
    delete *head;
    *head = nullptr;
}

TEST(CdllTests, TestInsert)
{
    cdll::Node* list = nullptr;
    cdll::insert(&list, 1);
    EXPECT_NE(list, nullptr);
    CheckConnections(list);
    DestroyTestList(&list);
}

TEST(CdllTests, TestInsertNullPointer)
{
    EXPECT_THROW(cdll::insert(nullptr, 1), std::runtime_error);
}

TEST(CdllTests, TestRemoveOne)
{
    cdll::Node* list = nullptr;
    cdll::insert(&list, 1);
    cdll::remove(&list, 1);
    EXPECT_EQ(list, nullptr);
    DestroyTestList(&list);
}

TEST(CdllTests, TestRemoveUnexpectedKey)
{
    cdll::Node* list = nullptr;
    for (int i : { 1, 2, 3, 4, 5, 6, 7, 8 })
        cdll::insert(&list, i);
        
    EXPECT_THROW(cdll::remove(&list, 666), std::runtime_error);
    DestroyTestList(&list);
}

TEST(CdllTests, TestRemoveFromEmptyList)
{
    cdll::Node* list = nullptr;

    EXPECT_THROW(cdll::remove(&list, 7), std::runtime_error);
    DestroyTestList(&list);
}

TEST(CdllTests, TestRemoveFromNullPointer)
{
    EXPECT_THROW(cdll::remove(nullptr, 7), std::runtime_error);
}

TEST(CdllTests, TestAddAndRemoveMultipleItems)
{
    cdll::Node* list = nullptr;
    cdll::insert(&list, 1);
    cdll::insert(&list, 1);

    cdll::remove(&list, 1); // Remove the first '1'
    cdll::remove(&list, 1); // Remove the second '1'

    EXPECT_EQ(list, nullptr);
    DestroyTestList(&list);
}

TEST(CdllTests, TestMultipleInsertions)
{
    cdll::Node* list = nullptr;

    for (int i : { 1, 2, 3, 4, 5, 6, 7, 8 })
        cdll::insert(&list, i);

    CheckConnections(list);

    DestroyTestList(&list);
}

TEST(CdllTests, TestMultipleInsertionsAndRemovals)
{
    cdll::Node* list = nullptr;

    auto keys_set_1 = { 1, 1, 2, 1, 2 };
    auto keys_set_2 = { 2, 3, 3, 1, 2 };
    for (int i : keys_set_1)
        cdll::insert(&list, i);
    for (int i : keys_set_2)
        cdll::insert(&list, i);
    
    // Remove the sets out of order
    for (int i : keys_set_2)
        cdll::remove(&list, i);

    CheckConnections(list);

    for (int i : keys_set_1)
        cdll::remove(&list, i);    

    CheckConnections(list);

    DestroyTestList(&list);
}

TEST(CdllTests, TestInvalidMultipleRemovals)
{
    cdll::Node* list = nullptr;

    for (int i : { 1, 2, 1, 2, 1, 2, 1 })
        cdll::insert(&list, i);
    
    // Try to remove 4 '2's
    for (int i : { 2, 2, 2 })
        cdll::remove(&list, i); // Should be OK

    EXPECT_THROW(cdll::remove(&list, 2), std::runtime_error);

    CheckConnections(list);
    DestroyTestList(&list);
}

std::list<int> GetContents(cdll::Node* list)
{
    std::list<int> contents;
    cdll::Node* start = list;
    while (list)
    {
        // Add them in reverse because insertions are done before the head
        contents.push_front(list->key);
        list = list->next;
        if (list == start)
            break;
    }

    return contents;
};

TEST(CdllTests, TestContents)
{
    cdll::Node* list = nullptr;

    std::list<int> startingContents = { 1, 2, 2, 3, 3, 3, 4, 4, 4, 4 };
    for (int i : startingContents)
        cdll::insert(&list, i);

    auto contents = GetContents(list);

    // Check the values in the list are as expected, and in the same order
    EXPECT_EQ(contents, startingContents);

    // Remove '1'
    cdll::remove(&list, 1);
    contents = GetContents(list);
    std::list<int> expectedContents = { 2, 2, 3, 3, 3, 4, 4, 4, 4 };
    EXPECT_EQ(contents, expectedContents);

    // Remove a '3' and a '4'
    cdll::remove(&list, 3);
    cdll::remove(&list, 4);
    contents = GetContents(list);
    expectedContents = { 2, 2, 3, 3, 4, 4, 4 };
    EXPECT_EQ(contents, expectedContents);

    DestroyTestList(&list);
}

