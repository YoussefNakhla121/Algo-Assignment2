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

// Concrete implementation lives in ArcadiaEngine.cpp.
// Forward declaration so we can call its debug printer.
class ConcreteAuctionTree;

static void printConcreteTree(AuctionTree* t, const std::string& label) {
    std::cout << "Tree state (" << label << "):\n";
    ConcreteAuctionTree* concrete = static_cast<ConcreteAuctionTree*>(t);
    concrete->printTree();
    std::cout << "-----------------------------\n";
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

    printConcreteTree(tree, "after inserts");

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

    printConcreteTree(tree, "initial");

    // Delete a leaf
    tree->deleteItem(13);
    printConcreteTree(tree, "after delete leaf (13)");

    // Delete a node with one child (after previous deletion, 11 may have one)
    tree->deleteItem(11);
    printConcreteTree(tree, "after delete internal (11)");

    // Delete root (will cause restructure)
    tree->deleteItem(10);
    printConcreteTree(tree, "after delete root (10)");

    // Additional delete to cover two-child removal
    tree->deleteItem(12);
    printConcreteTree(tree, "after delete node with two children (12)");

    delete tree;
    std::cout << "[PASS] Delete leaf/internal/root cases\n";
}

static void testMassInsertDelete() {
    AuctionTree* tree = createAuctionTree();
    const int basePrice = 1000;
    for (int i = 0; i < 100; ++i) {
        tree->insertItem(1000 + i, basePrice + (i % 7)); // many duplicates on price
    }
    printConcreteTree(tree, "after 100 inserts");

    // Remove every other item ID
    for (int i = 0; i < 100; i += 2) {
        tree->deleteItem(1000 + i);
    }
    printConcreteTree(tree, "after deleting every other id");

    // Attempt to delete some missing IDs (should be no-op / no crash)
    tree->deleteItem(99999);
    tree->deleteItem(42);

    delete tree;
    std::cout << "[PASS] Mass insert/delete with duplicates and missing IDs\n";
}

// Focused test for tie-breaking (same price, different IDs)
static void testTieBreakingOrder() {
    AuctionTree* tree = createAuctionTree();
    int price = 500;
    // Insert in shuffled ID order
    tree->insertItem(30, price);
    tree->insertItem(10, price);
    tree->insertItem(20, price);
    tree->insertItem(40, price);

    printConcreteTree(tree, "tie-breaking (all price=500, ids=10,20,30,40)");

    delete tree;
    std::cout << "[PASS] Tie-breaking insert order (visual check)\n";
}

// Sequential inserts and deletes to stress rotations near root
static void testSequentialOps() {
    AuctionTree* tree = createAuctionTree();

    for (int i = 0; i < 15; ++i) {
        tree->insertItem(100 + i, 10 * i);
    }
    printConcreteTree(tree, "after sequential increasing inserts");

    // Delete from middle, then ends
    tree->deleteItem(100 + 7);
    printConcreteTree(tree, "after deleting middle id (107)");

    tree->deleteItem(100);      // smallest
    tree->deleteItem(100 + 14); // largest
    printConcreteTree(tree, "after deleting smallest and largest");

    delete tree;
    std::cout << "[PASS] Sequential insert/delete stress\n";
}

int main() {
    std::cout << "AuctionTree focused tests\n";
    std::cout << "-------------------------\n";
    testInserts();
    testDeletesLeafInternalRoot();
    testMassInsertDelete();
    testTieBreakingOrder();
    testSequentialOps();
    std::cout << "All AuctionTree tests completed.\n";
    return 0;
}

