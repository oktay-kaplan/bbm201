#include "SpaceSectorLLRBT.h"
#include <queue>
#include <stack>


using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string& filename) {
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    int x, y, z;
    char comma;
    std::string line;
    std::getline(inputFile, line);

    while (inputFile >> x >> comma >> y >> comma >> z) {
        root = insertSectorByCoordinates(root, x, y, z, nullptr);
        root->color = BLACK;
    }

    inputFile.close();
}

// Implement the recursive insert function for LLRBT
Sector* SpaceSectorLLRBT::insertSectorByCoordinates(Sector* currentNode, int x, int y, int z, Sector* parent) {
    if (currentNode == nullptr) {
        Sector* newSector = new Sector(x, y, z);
        newSector->parent = parent;
        newSector->color = RED;
        return newSector;
    }

    if (x < currentNode->x || (x == currentNode->x && y < currentNode->y) ||
        (x == currentNode->x && y == currentNode->y && z < currentNode->z)) {
        currentNode->left = insertSectorByCoordinates(currentNode->left, x, y, z, currentNode);
    } else {
        currentNode->right = insertSectorByCoordinates(currentNode->right, x, y, z, currentNode);
    }

    // Fix any violations after insertion
    currentNode = fixViolations(currentNode);

    return currentNode;
}

// Implement the insertSectorByCoordinates method
void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    root = insertSectorByCoordinates(root, x, y, z, nullptr);
    root->color = BLACK;
}


Sector* SpaceSectorLLRBT::fixViolations(Sector* node) {
    // Case: Right-leaning red link
    if (isRed(node->right)) {
        node = rotateLeft(node);
    }

    // Case: Consecutive red links
    if (isRed(node->left) && isRed(node->left->left)) {
        node = rotateRight(node);
    }

    // Case: Split 4-node
    if (isRed(node->left) && isRed(node->right)) {
        flipColors(node);
    }

    // Update parent pointers
    if (node->left != nullptr) {
        node->left->parent = node;
    }
    if (node->right != nullptr) {
        node->right->parent = node;
    }

    return node;
}

Sector* SpaceSectorLLRBT::rotateLeft(Sector* node) {
    Sector* temp = node->right;
    node->right = temp->left;
    if (temp->left != nullptr) {
        temp->left->parent = node;
    }
    temp->left = node;
    temp->parent = node->parent;
    node->parent = temp;
    temp->color = node->color;
    node->color = RED;
    return temp;
}

Sector* SpaceSectorLLRBT::rotateRight(Sector* node) {
    Sector* temp = node->left;
    node->left = temp->right;
    if (temp->right != nullptr) {
        temp->right->parent = node;
    }
    temp->right = node;
    temp->parent = node->parent;
    node->parent = temp;
    temp->color = node->color;
    node->color = RED;
    return temp;
}

void SpaceSectorLLRBT::flipColors(Sector* node) {
    node->color = RED;
    node->left->color = BLACK;
    node->right->color = BLACK;
}

bool SpaceSectorLLRBT::isRed(Sector* node) const {
    return (node != nullptr) && (node->color == RED);
}


// Destructor
SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    // Call a helper function to perform post-order traversal and delete nodes
    destroyTree(root);
}

// Helper function to destroy the tree in post-order
void SpaceSectorLLRBT::destroyTree(Sector* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void SpaceSectorLLRBT::displaySectorsInOrder() {
    std::cout << "Space sectors inorder traversal: "<<std::endl;
    displaySectorsInOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::displaySectorsInOrderRecursive(Sector* node) {
    if (node != nullptr) {
        displaySectorsInOrderRecursive(node->left);
        string a = node->color?"RED":"BLACK";
        std::cout << a << " sector: " << node->sector_code  <<std::endl;
        displaySectorsInOrderRecursive(node->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    std::cout << "Space sectors preorder traversal: "<<std::endl;
    displaySectorsPreOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::displaySectorsPreOrderRecursive(Sector* node) {
    if (node != nullptr) {
        string a = node->color?"RED":"BLACK";
        std::cout << a << " sector: " << node->sector_code  <<std::endl;
        displaySectorsPreOrderRecursive(node->left);
        displaySectorsPreOrderRecursive(node->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    std::cout << "Space sectors postorder traversal: "<<std::endl;
    displaySectorsPostOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::displaySectorsPostOrderRecursive(Sector* node) {
    if (node != nullptr) {
        displaySectorsPostOrderRecursive(node->left);
        displaySectorsPostOrderRecursive(node->right);
        string a = node->color?"RED":"BLACK";
        std::cout << a << " sector: " << node->sector_code  <<std::endl;
    }
}

bool SpaceSectorLLRBT::getPath(Sector* root, vector<string>& arr, string x){
    if (!root)
        return false;

    arr.push_back(root->sector_code);

    if (root->sector_code == x)
        return true;
    if (getPath(root->left, arr, x) || getPath(root->right, arr, x))
        return true;

    arr.pop_back();
    return false;
}
std::vector<Sector*> SpaceSectorLLRBT::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;

    Sector* destinationSector = findSectorByCode( sector_code);
    if(!destinationSector)
        return path;

    vector<string> path1;

    vector<string> path2;

    getPath(root, path1, "0SSS");
    getPath(root, path2, sector_code);

    int intersection = -1;

    // Get intersection point
    int i = 0, j = 0;
    while (i != path1.size() || j != path2.size()) {

        // Keep moving forward until no intersection
        // is found
        if (i == j && path1[i] == path2[j]) {
            i++;
            j++;
        }
        else {
            intersection = j - 1;
            break;
        }
    }

    for (int i = path1.size() - 1; i > intersection; i--)
        path.push_back(findSectorByCode(path1[i]));

    for (int i = intersection; i < path2.size(); i++)
        path.push_back(findSectorByCode(path2[i]));


    return path;
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector*>& path) {
    if (path.empty()) {
        std::cout << "A path to Dr. Elara could not be found." << std::endl;
    } else {
        std::cout << "The stellar path to Dr. Elara: ";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i]->sector_code;
            if (i < path.size() - 1) {
                std::cout << "->";
            }
        }
        std::cout << std::endl;
    }
}
Sector* SpaceSectorLLRBT::findSectorByCodeBFS(Sector* startNode, const std::string& sector_code) const {
    if (startNode == nullptr) {
        return nullptr;
    }

    std::queue<Sector*> q;
    q.push(startNode);

    while (!q.empty()) {
        Sector* currentNode = q.front();
        q.pop();

        if (currentNode->sector_code == sector_code) {
            return currentNode;
        }

        if (currentNode->left != nullptr) {
            q.push(currentNode->left);
        }

        if (currentNode->right != nullptr) {
            q.push(currentNode->right);
        }
    }

    return nullptr;
}

Sector* SpaceSectorLLRBT::findSectorByCode(const std::string& sector_code) const {
    return findSectorByCodeBFS(root, sector_code);
}

