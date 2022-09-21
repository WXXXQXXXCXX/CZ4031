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
    void removeInternal(int, Node*, Node*);
    Node* findParent(Node*, Node*);

public:
    BPTree();
    void insert(int);
    void remove(int);
    Node* getRoot();
    void printKeys();
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
}

void BPTree::remove(int x) {
    if (root == NULL) {
        cout << "Tree empty\n";
    } else {
        Node *cursor = root;

        Node *parent;

        int leftSibling, rightSibling;

        while (cursor->IS_LEAF == false) {
            for (int i = 0; i < cursor->size; i++) {
                parent = cursor;
                leftSibling = i - 1;
                rightSibling = i + 1;

                //Finding if the key is within root note
                if (x < cursor->key[i]) {
                    cursor = cursor->ptr[i];
                    break;
                }
                //If x is larger than keys in root node
                if (i == cursor->size - 1) {
                    leftSibling = i;
                    rightSibling = i + 2;
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        bool found = false;
        int pos;
        //Finding exact location of key
        for (pos = 0; pos < cursor->size; pos++) {
            if (cursor->key[pos] == x) {
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Not found\n";
            return;
        }
        // Getting the next key
        for (int i = pos; i < cursor->size; i++) {
            cursor->key[i] = cursor->key[i + 1];
        }
        // Reducing node size since key is going to be removed
        cursor->size--;
        // removing root
        if (cursor == root) {
            for (int i = 0; i < MAX + 1; i++) {
                cursor->ptr[i] = NULL;
            }
            if (cursor->size == 0) {
                cout << "Tree died\n";
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                root = NULL;
            }
            return;
        }

        cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];

        cursor->ptr[cursor->size + 1] = NULL;

        if (cursor->size >= (MAX + 1) / 2) {
            return;
        }

        // Reorganising keys in internal and leaf nodes
        if (leftSibling >= 0) {
            Node *leftNode = parent->ptr[leftSibling];
            if (leftNode->size >= (MAX + 1) / 2 + 1) {
                cout<< cursor->size<<endl;
                for (int i = cursor->size; i > 0; i--) {
                    cursor->key[i] = cursor->key[i - 1];
                }
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[0] = leftNode->key[leftNode->size - 1];
                leftNode->size--;
                leftNode->ptr[leftNode->size] = cursor;
                leftNode->ptr[leftNode->size + 1] = NULL;
                parent->key[leftSibling] = cursor->key[0];
                return;
            }
        }

        if (rightSibling <= parent->size) {
            Node *rightNode = parent->ptr[rightSibling];
            if (rightNode->size >= (MAX + 1) / 2 + 1) {
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[cursor->size - 1] = rightNode->key[0];
                rightNode->size--;
                rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
                rightNode->ptr[rightNode->size + 1] = NULL;
                for (int i = 0; i < rightNode->size; i++) {
                    rightNode->key[i] = rightNode->key[i + 1];
                }
                parent->key[rightSibling - 1] = rightNode->key[0];
                return;
            }
        }

        if (leftSibling >= 0) {
            Node *leftNode = parent->ptr[leftSibling];
            for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++) {
                leftNode->key[i] = cursor->key[j];
            }
            leftNode->ptr[leftNode->size] = NULL;
            leftNode->size += cursor->size;
            leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];
            removeInternal(parent->key[leftSibling], parent, cursor);
            delete[] cursor->key;
            delete[] cursor->ptr;
            delete cursor;
        } else if (rightSibling <= parent->size) {
            Node *rightNode = parent->ptr[rightSibling];
            for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++) {
                cursor->key[i] = rightNode->key[j];
            }
            cursor->ptr[cursor->size] = NULL;
            cursor->size += rightNode->size;
            cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
            cout << "Merging two leaf nodes\n";
            removeInternal(parent->key[rightSibling - 1], parent, rightNode);
            delete[] rightNode->key;
            delete[] rightNode->ptr;
            delete rightNode;
        }
    }
}

void BPTree::removeInternal(int x, Node *cursor, Node *child) {
    if (cursor == root) {
        if (cursor->size == 1) {
            if (cursor->ptr[1] == child) {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                root = cursor->ptr[0];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Changed root node\n";
                return;
            } else if (cursor->ptr[0] == child) {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                root = cursor->ptr[1];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Changed root node\n";
                return;
            }
        }
    }

    int pos;

    for (pos = 0; pos < cursor->size; pos++) {
        if (cursor->key[pos] == x) {
            break;
        }
    }

    for (int i = pos; i < cursor->size; i++) {
        cursor->key[i] = cursor->key[i + 1];
    }

    for (pos = 0; pos < cursor->size + 1; pos++) {
        if (cursor->ptr[pos] == child) {
            break;
        }
    }

    for (int i = pos; i < cursor->size + 1; i++) {
        cursor->ptr[i] = cursor->ptr[i + 1];
    }

    cursor->size--;

    if (cursor->size >= (MAX + 1) / 2 - 1) {
        return;
    }

    if (cursor == root)
        return;

    Node *parent = findParent(root, cursor);

    int leftSibling, rightSibling;

    for (pos = 0; pos < parent->size + 1; pos++) {
        if (parent->ptr[pos] == cursor) {
            leftSibling = pos - 1;
            rightSibling = pos + 1;
            break;
        }
    }

    if (leftSibling >= 0) {
        Node *leftNode = parent->ptr[leftSibling];
        if (leftNode->size >= (MAX + 1) / 2) {
        for (int i = cursor->size; i > 0; i--) {
            cursor->key[i] = cursor->key[i - 1];
        }
        cursor->key[0] = parent->key[leftSibling];
        parent->key[leftSibling] = leftNode->key[leftNode->size - 1];
        for (int i = cursor->size + 1; i > 0; i--) {
            cursor->ptr[i] = cursor->ptr[i - 1];
        }
        cursor->ptr[0] = leftNode->ptr[leftNode->size];
        cursor->size++;
        leftNode->size--;
        return;
        }
    }

    if (rightSibling <= parent->size) {
        Node *rightNode = parent->ptr[rightSibling];
        if (rightNode->size >= (MAX + 1) / 2) {
            cursor->key[cursor->size] = parent->key[pos];
            parent->key[pos] = rightNode->key[0];
            for (int i = 0; i < rightNode->size - 1; i++) {
                rightNode->key[i] = rightNode->key[i + 1];
            }
            cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
            for (int i = 0; i < rightNode->size; ++i) {
                rightNode->ptr[i] = rightNode->ptr[i + 1];
            }
            cursor->size++;
            rightNode->size--;
            return;
        }
    }

    if (leftSibling >= 0) {
        Node *leftNode = parent->ptr[leftSibling];
        leftNode->key[leftNode->size] = parent->key[leftSibling];
        for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++) {
            leftNode->key[i] = cursor->key[j];
        }
        for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++) {
            leftNode->ptr[i] = cursor->ptr[j];
            cursor->ptr[j] = NULL;
        }
        leftNode->size += cursor->size + 1;
        cursor->size = 0;
        removeInternal(parent->key[leftSibling], parent, cursor);
    } else if (rightSibling <= parent->size) {
        Node *rightNode = parent->ptr[rightSibling];
        cursor->key[cursor->size] = parent->key[rightSibling - 1];
        for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++) {
            cursor->key[i] = rightNode->key[j];
        }
        for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++) {
            cursor->ptr[i] = rightNode->ptr[j];
            rightNode->ptr[j] = NULL;
        }
        cursor->size += rightNode->size + 1;
        rightNode->size = 0;
        removeInternal(parent->key[rightSibling - 1], parent, rightNode);
    }
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
    node.remove(26);

    node.printKeys();

    return 0;
}
