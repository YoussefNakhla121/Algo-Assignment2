// Simple manual tests for AuctionTree
// Build example (MSVC/Clang/GCC):
//   g++ -std=c++17 ArcadiaEngine.cpp test_auctiontree.cpp -o test_auction

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "ArcadiaEngine.h"

// Factory from ArcadiaEngine.cpp
extern "C" {
    AuctionTree* createAuctionTree();
}

static void testInserts() {
    AuctionTree* tree = createAuctionTree();
    // Distinct prices
    tree->insertItem(1, 100);
    tree->insertItem(2, 50);
    tree->insertItem(3, 150);
    // Tie on price; ordering rule uses itemID ascending
    tree->insertItem(4, 100);
    tree->insertItem(5, 100);
    // If no crashes/rebalances issues, test passes
    delete tree;
    std::cout << "[PASS] Insert sequence\n";
}

static void testDeletesLeafInternalRoot() {
    AuctionTree* tree = createAuctionTree();
    // Build a small tree with varied structure
    tree->insertItem(10, 100);  // root candidate
    tree->insertItem(11, 50);   // left
    tree->insertItem(12, 150);  // right
    tree->insertItem(13, 25);   // leaf
    tree->insertItem(14, 75);   // internal in left subtree
    tree->insertItem(15, 125);  // internal in right subtree

    // Delete a leaf
    tree->deleteItem(13);
    // Delete a node with one child (after previous deletion, 11 may have one)
    tree->deleteItem(11);
    // Delete root (will cause restructure)
    tree->deleteItem(10);
    // Additional delete to cover two-child removal
    tree->deleteItem(12);

    delete tree;
    std::cout << "[PASS] Delete leaf/internal/root cases\n";
}

static void testMassInsertDelete() {
    AuctionTree* tree = createAuctionTree();
    const int basePrice = 1000;
    for (int i = 0; i < 100; ++i) {
        tree->insertItem(1000 + i, basePrice + (i % 7)); // many duplicates on price
    }
    // Remove every other item ID
    for (int i = 0; i < 100; i += 2) {
        tree->deleteItem(1000 + i);
    }
    // Attempt to delete some missing IDs (should be no-op / no crash)
    tree->deleteItem(99999);
    tree->deleteItem(42);

    delete tree;
    std::cout << "[PASS] Mass insert/delete with duplicates and missing IDs\n";
}

int main() {
    std::cout << "AuctionTree focused tests\n";
    std::cout << "-------------------------\n";
    testInserts();
    testDeletesLeafInternalRoot();
    testMassInsertDelete();
    std::cout << "All AuctionTree tests completed.\n";
    return 0;
}

