// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

enum type{allowed,notallowed};

struct HashEntry{
    string element;
    int playerID;
    type status;
};


class ConcretePlayerTable : public PlayerTable {
private:
    int sz=101;
    HashEntry Table[101];
    int currentsize=0;
    int hashfunction1(int key)
    {
        return key%sz;
    }
    int hashfunction2(int key)
    {
        int prime=97;
        return prime-(key%prime);
    }

    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution

public:
    ConcretePlayerTable() {
        //Table=new HashEntry*[sz];
        for(int i=0;i<sz;i++)
        {
            Table[i].element=nullptr;
            Table[i].playerID=0;
            Table[i].status=allowed;
        }
        // TODO: Initialize your hash table
    }

    void insert(int playerID, string name) override {
        if(currentsize>=sz)
        {
            cout<<"The Table is full."<<endl;
        }
        int i=0;
        int position=(hashfunction1(playerID)+(i*hashfunction2(playerID)))%sz;
        while(Table[position].status==notallowed)
        {
            i++;
            if(Table[position].playerID==playerID)
            {
                cout<<"This player is already existed."<<endl;
            }
            position=(hashfunction1(playerID)+(i*hashfunction2(playerID)))%sz;

        }
        Table[position].element=name;
        Table[position].playerID=playerID;
        Table[position].status=notallowed;
        currentsize++;
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
    }

    string search(int playerID) override {
        if(currentsize==0)
        {
            cout<<"The Table is Empty."<<endl;
        }
        int i=0;
        int position=(hashfunction1(playerID)+(i*hashfunction2(playerID)))%sz;
        while(Table[position].status==allowed&&Table[position].playerID!=playerID)
        {
            i++;
            position=(hashfunction1(playerID)+(i*hashfunction2(playerID)))%sz;
            if(i>101)
                break;
        }
        if(i>101)
        {
            Table[position].status=allowed;
            return Table[position].element;
        }
        else
            return "";
        // TODO: Implement double hashing search
        // Return "" if player not found
    }
};

// --- 2. Leaderboard (Skip List) ---
struct Node
{
    Node *next;
    Node *down;
    int playerID;
    int score;
    int level;
    Node(Node *next,Node *down,int playerID,int score,int level)
    {
        this->next=next;
        this->down=down;
        this->playerID=playerID;
        this->score=score;
        this->level=level;
    }
};
class ConcreteLeaderboard : public Leaderboard {
private:

    Node *head;
    int currentsize=0;
    int getrandomlevel()
    {
        int level=0;
        while(level<=head->level&&(rand()%2==1))
        {
            level++;
        }
        return level;
    }
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

public:
    ConcreteLeaderboard() {
        head=new Node(NULL,NULL,0,0,0);
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override {
        int insertlevel=getrandomlevel();
        if(insertlevel>head->level)
        {
            head=new Node(NULL,head,0,0,insertlevel);
        }
        Node* cur=head;
        Node* bottom=NULL;
        while(cur!=NULL)
        {
            Node* next=cur->next;
            if(next==NULL||next->playerID<playerID)
            {
                if(cur->level<=insertlevel)
                {
                    Node* player=new Node(next,NULL,playerID,score,cur->level);
                    currentsize++;
                    cur->next=player;
                    if(bottom!=NULL)
                        bottom->down=player;
                    bottom=player;
                }
                cur=cur->down;
            }
            else
                cur=cur->next;
        }
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override {
        Node* cur=head;
        vector<Node*>currents;
        while(cur!=NULL)
        {
            currents.push_back(cur);
            cur=cur->down;
        }
        for(int i=0;i<currents.size();i++)
        {
            Node *current=currents[i];
            Node *next=current->next;
            while(next!=NULL&&next->playerID!=playerID)
            {
                next=next->next;
                current=current->next;
            }
            Node *deletenode=next;
            current->next=next->next;
            delete deletenode;
        }
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override {
        vector<int>topn;
        Node *cur=head;
        int skipedplayers=currentsize-n;
        while(cur->next!=NULL)
        {
            if(skipedplayers==0)
                topn.push_back(cur->playerID);
            else
                skipedplayers--;
        }
        for(int i=0;i<topn.size()/2;i++)
        {
            swap(topn[i],topn[topn.size()-i-1]);
        }
        // TODO: Return top N player IDs in descending score order
        return topn;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    enum Color { RED, BLACK };

    struct Node {
        int itemID;
        int price;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
    };

    Node* root;
    Node sentinel; // shared NIL sentinel to avoid nullptr checks
    Node* NIL;

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == NIL) {
            root = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    Node* treeMinimum(Node* x) {
        while (x->left != NIL) {
            x = x->left;
        }
        return x;
    }

    void fixInsert(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right; // uncle
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left; // uncle
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void fixDelete(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right; // sibling
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotateLeft(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rotateRight(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    rotateLeft(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left; // sibling
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotateRight(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        rotateLeft(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rotateRight(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    Node* findByID(int itemID) {
        if (root == NIL) return NIL;
        vector<Node*> stack;
        stack.push_back(root);
        while (!stack.empty()) {
            Node* cur = stack.back();
            stack.pop_back();
            if (cur->itemID == itemID) return cur;
            if (cur->left != NIL) stack.push_back(cur->left);
            if (cur->right != NIL) stack.push_back(cur->right);
        }
        return NIL;
    }

    void printTreeHelper(Node* node, int depth) {
        if (node == NIL) return;
        printTreeHelper(node->left, depth + 1);
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        cout << "[price=" << node->price
            << ", id=" << node->itemID
            << ", color=" << (node->color == RED ? "R" : "B") << "]\n";
        printTreeHelper(node->right, depth + 1);
    }

public:
    ConcreteAuctionTree() {
        sentinel.color = BLACK;
        sentinel.left = &sentinel;
        sentinel.right = &sentinel;
        sentinel.parent = &sentinel;
        sentinel.itemID = -1;
        sentinel.price = -1;
        NIL = &sentinel;
        root = NIL;
    }

    // Debugging utility: prints the tree in-order with indentation.
    // Not part of the public interface in ArcadiaEngine.h, but can be
    // used directly if you have a ConcreteAuctionTree instance.
    void printTree() {
        if (root == NIL) {
            cout << "(empty tree)\n";
            return;
        }
        printTreeHelper(root, 0);
    }

    void insertItem(int itemID, int price) override {
        Node* z = new Node{itemID, price, RED, NIL, NIL, NIL};

        Node* y = NIL;
        Node* x = root;

        while (x != NIL) {
            y = x;
            if (price < x->price) {
                x = x->left;
            } else if (price > x->price) {
                x = x->right;
            } else {
                if (itemID < x->itemID) {
                    x = x->left;
                } else {
                    x = x->right;
                }
            }
        }

        z->parent = y;
        if (y == NIL) {
            root = z;
        } else if (price < y->price || (price == y->price && itemID < y->itemID)) {
            y->left = z;
        } else {
            y->right = z;
        }

        fixInsert(z);
    }

    void deleteItem(int itemID) override {
        Node* z = findByID(itemID);
        if (z == NIL) return;

        Node* y = z;
        Color yOriginalColor = y->color;
        Node* x = NIL;

        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = treeMinimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;

        if (yOriginalColor == BLACK) {
            fixDelete(x);
        }
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {

    long long total = 0;
    for (int i = 0; i < n; ++i) total += coins[i];

    if (n == 0) return 0;

    int half = (total / 2);
    vector<char> dp(half + 1, 0);
    dp[0] = 1;

    for (int i = 0; i < n; ++i) {
        int v = coins[i];
        //if (v <= 0) continue
        if (v > half) continue;

        for (int s = half; s >= v; --s) {
            if (dp[s - v]) dp[s] = 1;
        }

    }

    for (int s = half; s >= 0; --s) {
        if (dp[s]) {
            long long diff = total - 2 * s;
            return diff;
        }
    }

    return total;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();

    if (capacity <= 0 || n == 0) return 0;

    vector<vector<int>> V(n + 1, vector<int>(capacity + 1, 0));
    //vector<vector<int>> P(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; ++i) {
        int w = items[i - 1].first;
        int val = items[i - 1].second;

        for (int j = 1; j <= capacity; ++j) {
            if (w <= j && val + V[i - 1][j - w] > V[i - 1][j]) {
                V[i][j] = val + V[i - 1][j - w];
                //P[i][j] = j - w;
            } else {
                V[i][j] = V[i - 1][j];
               // P[i][j] = j;
            }
        }
    }

    return V[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    int n = s.size();
    if (n == 0) return 0;
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; ++i) {
        dp[i] = (dp[i] + dp[i - 1]);


        if (i >= 2) {
            if (s[i - 1] == s[i - 2] && (s[i - 1] == 'u' || s[i - 1] == 'n')) {
                dp[i] = (dp[i] + dp[i - 2]);
            }
        }
    }

    return dp[n] ;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() {
        return new ConcretePlayerTable();
    }

    Leaderboard* createLeaderboard() {
        return new ConcreteLeaderboard();
    }

    AuctionTree* createAuctionTree() {
        return new ConcreteAuctionTree();
    }
}
