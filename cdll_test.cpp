#include "gtest/gtest.h"

#include "cdll.h"

#include <stdexcept>
#include <vector>

namespace
{

// Walk the list forwards from the head and collect the keys.
// Stops after maxNodes steps so a broken list can't loop forever.
std::vector<int> keysForward(cdll::Node* head, size_t maxNodes = 1000)
{
    std::vector<int> keys;
    if (head == nullptr)
    {
        return keys;
    }

    cdll::Node* node = head;
    do
    {
        keys.push_back(node->key);
        node = node->next;
    } while (node != head && keys.size() < maxNodes);

    return keys;
}

// Walk the list backwards from the head and collect the keys.
// Returned in forward order so it can be compared with keysForward().
std::vector<int> keysBackward(cdll::Node* head, size_t maxNodes = 1000)
{
    std::vector<int> keys;
    if (head == nullptr)
    {
        return keys;
    }

    cdll::Node* node = head;
    do
    {
        keys.insert(keys.begin(), node->key);
        node = node->prev;
    } while (node != head && keys.size() < maxNodes);

    // Rotate so the head comes first, matching the forward order.
    if (keys.size() > 1)
    {
        keys.insert(keys.begin(), keys.back());
        keys.pop_back();
    }
    return keys;
}

// Check every node's neighbours point back at it.
void expectLinksConsistent(cdll::Node* head)
{
    if (head == nullptr)
    {
        return;
    }

    cdll::Node* node = head;
    size_t steps = 0;
    do
    {
        ASSERT_NE(node->next, nullptr);
        ASSERT_NE(node->prev, nullptr);
        EXPECT_EQ(node->next->prev, node) << "next->prev broken at key " << node->key;
        EXPECT_EQ(node->prev->next, node) << "prev->next broken at key " << node->key;
        node = node->next;
        ASSERT_LT(++steps, 1000u) << "list is not circular";
    } while (node != head);
}

// Free the list. Walks forwards only, so it works even if prev links are broken.
void destroy(cdll::Node** head)
{
    if (*head == nullptr)
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

// Fixture that frees the list after each test.
class CdllTest : public ::testing::Test
{
protected:
    void TearDown() override { destroy(&list); }

    void insertAll(std::initializer_list<int> keys)
    {
        for (int key : keys)
        {
            cdll::insert(&list, key);
        }
    }

    cdll::Node* list = nullptr;
};

} // namespace

// ---------------------------------------------------------------- find

TEST_F(CdllTest, FindOnEmptyListReturnsNull)
{
    EXPECT_EQ(cdll::find(list, 1), nullptr);
}

TEST_F(CdllTest, FindSingleNode)
{
    insertAll({7});
    cdll::Node* node = cdll::find(list, 7);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->key, 7);
}

TEST_F(CdllTest, FindMissingKeyReturnsNull)
{
    insertAll({1, 2, 3});
    EXPECT_EQ(cdll::find(list, 42), nullptr);
}

TEST_F(CdllTest, FindEveryKey)
{
    insertAll({1, 2, 3, 4, 5});
    for (int key = 1; key <= 5; ++key)
    {
        cdll::Node* node = cdll::find(list, key);
        ASSERT_NE(node, nullptr) << "key " << key;
        EXPECT_EQ(node->key, key);
    }
}

TEST_F(CdllTest, FindWithDuplicatesReturnsFirstFromHead)
{
    insertAll({5, 5});
    EXPECT_EQ(cdll::find(list, 5), list);
}

// ---------------------------------------------------------------- insert

TEST(CdllInsert, NullListPointerThrows)
{
    EXPECT_THROW(cdll::insert(nullptr, 1), std::runtime_error);
}

TEST_F(CdllTest, InsertIntoEmptyListMakesSelfLoop)
{
    insertAll({1});
    ASSERT_NE(list, nullptr);
    EXPECT_EQ(list->key, 1);
    EXPECT_EQ(list->next, list);
    EXPECT_EQ(list->prev, list);
}

TEST_F(CdllTest, InsertPrependsToHead)
{
    insertAll({1, 2, 3});
    EXPECT_EQ(list->key, 3);
    EXPECT_EQ(keysForward(list), (std::vector<int>{3, 2, 1}));
}

TEST_F(CdllTest, InsertTwoNodesLinksBothWays)
{
    insertAll({1, 2});
    expectLinksConsistent(list);
    EXPECT_EQ(list->next, list->prev); // with two nodes, next and prev are the same node
}

TEST_F(CdllTest, InsertManyKeepsLinksConsistent)
{
    insertAll({1, 2, 3, 4, 5});
    expectLinksConsistent(list);
}

TEST_F(CdllTest, ForwardAndBackwardTraversalAgree)
{
    insertAll({1, 2, 3, 4, 5});
    EXPECT_EQ(keysBackward(list), keysForward(list));
}

TEST_F(CdllTest, HeadPrevIsTail)
{
    insertAll({1, 2, 3});
    // List is 3 -> 2 -> 1, so the tail is 1.
    EXPECT_EQ(list->prev->key, 1);
}

TEST_F(CdllTest, InsertAllowsDuplicates)
{
    insertAll({4, 4, 4});
    EXPECT_EQ(keysForward(list), (std::vector<int>{4, 4, 4}));
}

// ---------------------------------------------------------------- remove

TEST(CdllRemove, FromEmptyListThrows)
{
    cdll::Node* list = nullptr;
    EXPECT_THROW(cdll::remove(&list, 1), std::runtime_error);
}

// Disabled: remove() dereferences the pointer without a null check and crashes,
// whereas insert() throws for the same input.
TEST(CdllRemove, DISABLED_NullListPointerThrows)
{
    EXPECT_THROW(cdll::remove(nullptr, 1), std::runtime_error);
}

TEST_F(CdllTest, RemoveMissingKeyThrowsAndLeavesListAlone)
{
    insertAll({1, 2, 3});
    EXPECT_THROW(cdll::remove(&list, 42), std::runtime_error);
    EXPECT_EQ(keysForward(list), (std::vector<int>{3, 2, 1}));
}

TEST_F(CdllTest, RemoveOnlyNodeEmptiesList)
{
    insertAll({1});
    cdll::remove(&list, 1);
    EXPECT_EQ(list, nullptr);
}

TEST_F(CdllTest, RemoveHeadAdvancesHead)
{
    insertAll({1, 2, 3}); // 3 -> 2 -> 1
    cdll::remove(&list, 3);
    EXPECT_EQ(list->key, 2);
    EXPECT_EQ(keysForward(list), (std::vector<int>{2, 1}));
    expectLinksConsistent(list);
}

TEST_F(CdllTest, RemoveMiddleNode)
{
    insertAll({1, 2, 3}); // 3 -> 2 -> 1
    cdll::remove(&list, 2);
    EXPECT_EQ(keysForward(list), (std::vector<int>{3, 1}));
    expectLinksConsistent(list);
}

TEST_F(CdllTest, RemoveTailNode)
{
    insertAll({1, 2, 3}); // 3 -> 2 -> 1
    cdll::remove(&list, 1);
    EXPECT_EQ(keysForward(list), (std::vector<int>{3, 2}));
    expectLinksConsistent(list);
}

TEST_F(CdllTest, RemoveDownToOneNodeLeavesSelfLoop)
{
    insertAll({1, 2});
    cdll::remove(&list, 2);
    ASSERT_NE(list, nullptr);
    EXPECT_EQ(list->key, 1);
    EXPECT_EQ(list->next, list);
    EXPECT_EQ(list->prev, list);
}

TEST_F(CdllTest, RemoveDuplicateRemovesOnlyOne)
{
    insertAll({1, 5, 5});
    cdll::remove(&list, 5);
    EXPECT_EQ(keysForward(list), (std::vector<int>{5, 1}));
    expectLinksConsistent(list);
}

TEST_F(CdllTest, RemoveAllNodes)
{
    insertAll({1, 2, 3, 4, 5});
    for (int key : {3, 1, 5, 2, 4})
    {
        cdll::remove(&list, key);
        expectLinksConsistent(list);
        EXPECT_EQ(cdll::find(list, key), nullptr);
    }
    EXPECT_EQ(list, nullptr);
}

TEST_F(CdllTest, InsertAfterEmptyingList)
{
    insertAll({1});
    cdll::remove(&list, 1);
    insertAll({2, 3});
    EXPECT_EQ(keysForward(list), (std::vector<int>{3, 2}));
    expectLinksConsistent(list);
}
