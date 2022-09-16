// C++ program for implementing B+ Tree
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;
int MAX = 3;

// BP node
class Node {
    bool IS_LEAF;
    int *key, size;
    Node** ptr;
    friend class BPTree;

public:
    Node();
};

// BP tree
class BPTree {
    Node* root;
    void insertInternal(int, Node*, Node*);
    Node* findParent(Node*, Node*);

public:
    BPTree();
    void insert(int);
    Node* getRoot();
    void printKeys();
    void search(float, float);
};

// Constructor of Node
Node::Node()
{
    key = new int[MAX];
    ptr = new Node*[MAX + 1];

    for (int i = 0; i < MAX; i++){
        key[i] = 0;
    }

    for (int j = 0; j < MAX + 1; j++){
        ptr[j] = NULL;
    }
}

// Initialise the BPTree Node
BPTree::BPTree()
{
    root = NULL;
}



// Function to implement the Insert Operation in B+ Tree
void BPTree::insert(int x)
{

    // If root is null then return newly created node
    if (root == NULL) {
        root = new Node;
        root->key[0] = x;
        root->IS_LEAF = true;
        root->size = 1;
    }

        // Traverse the B+ Tree
    else {
        Node* current = root;
        Node* parent;

        // Till current reaches the leaf node
        while (current->IS_LEAF == false) {

            parent = current;

            for (int i = 0; i < current->size; i++) {

                // If found the position where we have to insert node
                if (x < current->key[i]) {
                    current = current->ptr[i];
                    break;
                }

                // If reaches the end
                if (i == current->size - 1) {
                    current = current->ptr[i + 1];
                    break;
                }
            }
        }

        // Current leaf node have space
        if (current->size < MAX) {
            int i = 0;
            // Find position of key in current leaf node
            while (x > current->key[i] && i < current->size) {
                i++;
            }

            // If x is inserted before other numbers, shift the bigger numbers back
            for (int j = current->size; j > i; j--) {
                current->key[j] = current->key[j - 1];
            }

            // Insert x into current leaf node and increase size of node
            current->key[i] = x;
            current->size++;

            // Shift pointer for next leaf node to the last pointer
            current->ptr[current->size] = current->ptr[current->size - 1];
            current->ptr[current->size - 1] = NULL;
        }


            // If current leaf node no space
        else {

            // Create a newLeaf node
            Node* newLeaf = new Node;
            newLeaf->IS_LEAF = true;

            // Create virtual node that can store 1 more key than original node
            int virtualNode[MAX + 1];

            // Update key in current node to virtual node created
            for (int i = 0; i < MAX; i++) {
                virtualNode[i] = current->key[i];
            }

            int i = 0, j;

            // Find position of key in current virtual node
            while (x > virtualNode[i] && i < MAX) {
                i++;
            }

            // If x is inserted before other numbers, shift the bigger numbers back
            for (int j = MAX + 1; j > i; j--) {
                virtualNode[j] = virtualNode[j - 1];
            }

            // Insert x into current virtual node
            virtualNode[i] = x;

            // Update size of current leaf node and new leaf node 
            current->size = (MAX + 1) / 2;
            newLeaf->size = MAX + 1 - (MAX + 1) / 2;

            // Update current leaf node to point at new leaf node
            current->ptr[current->size] = newLeaf;

            // Update new leaf node to point at the next leaf node
            newLeaf->ptr[newLeaf->size] = current->ptr[MAX];

            // Remove current leaf node poiner that is pointing towards the next next leaf node
            current->ptr[MAX] = NULL;

            // Update the current leaf node's key
            for (i = 0; i < current->size; i++) {
                current->key[i] = virtualNode[i];
            }

            // Update the new leaf node's key
            for (i = 0, j = current->size; i < newLeaf->size; i++, j++) {
                newLeaf->key[i] = virtualNode[j];
            }

            // If current is the root node
            if (current == root) {

                // Create a new Node
                Node* newRoot = new Node;

                // Update rest field of B+ Tree Node
                newRoot->key[0] = newLeaf->key[0];
                newRoot->ptr[0] = current;
                newRoot->ptr[1] = newLeaf;
                newRoot->IS_LEAF = false;
                newRoot->size = 1;
                root = newRoot;
            }
            else {

                // Recursive Call for insert in internal
                insertInternal(newLeaf->key[0], parent, newLeaf);
            }
        }
    }
}

// Function to implement the Insert Internal Operation in B+ Tree
void BPTree::insertInternal(int x, Node* current, Node* child){

    // If we doesn't have overflow
    if (current->size < MAX) {
        int i = 0;

        // Find position of key in current internal node
        while (x > current->key[i] && i < current->size) {
            i++;
        }

        // If x is inserted before other numbers, shift the bigger numbers back
        for (int j = current->size; j > i; j--) {
            current->key[j] = current->key[j - 1];
        }

        // If x is inserted before other numbers, shift the bigger numbers' pointers back
        for (int j = current->size + 1; j > i + 1; j--) {
            current->ptr[j] = current->ptr[j - 1];
        }

        // Update current internal node with the key and pointer to child
        current->key[i] = x;
        current->size++;
        current->ptr[i + 1] = child;
    }

        // For overflow, break the node
    else {

        // Create virtual internal node that can store 1 more key than original internal node
        Node* newInternal = new Node;
        newInternal->IS_LEAF = false;
        int virtualKey[MAX + 1];
        Node* virtualPtr[MAX + 2];

        // Update key in current internal node to virtual internal node created
        for (int i = 0; i < MAX; i++) {
            virtualKey[i] = current->key[i];
        }

        // Update pointers in current internal node to virtual internal node created   
        for (int i = 0; i < MAX + 1; i++) {
            virtualPtr[i] = current->ptr[i];
        }

        int i = 0, j;

        // Find position of key in current virtual internal node
        while (x > virtualKey[i] && i < MAX) {
            i++;
        }

        // If x is inserted before other numbers, shift the bigger numbers back
        for (int j = MAX + 1; j > i; j--) {
            virtualKey[j] = virtualKey[j - 1];
        }

        // Insert x into current internal node
        virtualKey[i] = x;

        // If x is inserted before other numbers, shift the bigger numbers' pointer back
        for (int j = MAX + 2; j > i + 1; j--) {
            virtualPtr[j] = virtualPtr[j - 1];
        }

        // Insert x's child node into current internal node
        virtualPtr[i + 1] = child;

        // Update size for both current node and new internal node
        current->size = (MAX + 1) / 2;

        newInternal->size = MAX - (MAX + 1) / 2;

        // Update key and pointers for new internal node
        for (i = 0, j = current->size + 1; i < newInternal->size; i++, j++) {
            newInternal->key[i] = virtualKey[j];
        }

        for (i = 0, j = current->size + 1; i < newInternal->size + 1; i++, j++) {
            newInternal->ptr[i] = virtualPtr[j];
        }

        // If current is the root node
        if (current == root) {

            // Create a new root node
            Node* newRoot = new Node;

            // Update key value
            newRoot->key[0] = current->key[current->size];

            // Update rest field of B+ Tree Node
            newRoot->ptr[0] = current;
            newRoot->ptr[1] = newInternal;
            newRoot->IS_LEAF = false;
            newRoot->size = 1;
            root = newRoot;
        }

        else {

            // Recursive Call to insert the data
            insertInternal(current->key[current->size], findParent(root, current), newInternal);
        }
    }
}

// Function to find the parent node
Node* BPTree::findParent(Node* current, Node* child)
{
    Node* parent;

    // If current reaches the end of Tree
    if (current->IS_LEAF || (current->ptr[0])->IS_LEAF) {
        return NULL;
    }

    // Traverse the current node with all its child
    for (int i = 0; i < current->size + 1; i++) {

        // Update the parent for the child Node
        if (current->ptr[i] == child) {
            parent = current;
            return parent;
        }

            // Else recursively traverse to find child node
        else {
            parent = findParent(current->ptr[i], child);

            // If parent is found, then return that parent node
            if (parent != NULL){
                return parent;
            }
        }
    }

    // Return parent node
    return parent;
}

// Function to get the root Node
Node* BPTree::getRoot()
{
    return root;
}

// Function to print keys
void BPTree::printKeys()
{
    Node* current;

    // If there is no root node in B+ Tree
    if (root == NULL){
        cout << "no keys in B+ Tree \n";
    }

        // If there is root node in B+ Tree
    else {

        // Set current node to root node
        current = root;

        // Traverse to the first leaf node
        while (current->IS_LEAF == false){
            current = current->ptr[0];
        }

        cout << "The keys are: \n";

        // While loop to print keys from all leaf node
        while (true){

            int i = 0;

            // Print all the keys that are in current leaf node
            while (i < current->size){
                cout << current->key[i] << " ";
                i++;
            }

            // Set current leaf node to the next leaf node
            current = current->ptr[current->size];

            // If the next leaf node is NULL means current leaf node is the last leaf node thus break out of loop
            if (current == NULL){
                break;
            }

        }
    }

    cout << endl;
}

// Function to search between a range of value
void BPTree::search(float lowerBound, float upperBound) {
    // if tree is empty
    if (root == nullptr) {
        cout << "no keys in B+ Tree" << endl;
        return;
    }

    cout << "The keys in the range [" << lowerBound << ", " << upperBound << "] are:" << endl;

    Node* current = root;

    // iteratively travel down internal node till we found the first leaf node that corresponds to lowerBound
    while (!current->IS_LEAF) {
        for (int i = 0; i < current->size; i++) {
            int currentKey = current->key[i];

            // Take the left pointer if lowerBound is smaller than the current key
            if (lowerBound < currentKey) {
                current = current->ptr[i];
                break;
            }

            // if lowerBound > all other keys in the current node,
            // we will take the (i + 1)th pointer
            if (i == current->size - 1) {
                current = current->ptr[i + 1];
            }
        }
    }

    // once we have located the first leaf node that corresponds to lowerBound,
    // iterate through each of the subsequent leaf node till we encounter out of range
    bool terminate = false;

    while (!terminate) {
        // search through the node to find the keys which corresponds to the given range
        for (int i = 0; i < current->size; i++) {
            int currentKey = current->key[i];

            if (currentKey > upperBound) {
                terminate = true;
                break;
            }

            if (currentKey >= lowerBound && currentKey <= upperBound) {
                cout << currentKey << " ";
            }
        }

        // travel to the next leaf node, if last key of the current leaf < upperBound
        if (current->ptr[current->size] != nullptr && current->key[current->size - 1] < upperBound) {
            current = current->ptr[current->size];
            continue;
        }

        break;
    }

    cout << endl;
}


// Driver Code
int main()
{
    BPTree node;

    // Create B+ Tree
    node.printKeys();
    node.insert(6);
    node.insert(16);
    node.insert(26);
    node.insert(36);
    node.insert(46);
    node.insert(56);
    node.printKeys();
    node.search(26, 88);

    return 0;
}