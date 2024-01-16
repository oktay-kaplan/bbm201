#include "SpaceSectorBST.h"
#include <queue>

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

// Destructor
SpaceSectorBST::~SpaceSectorBST() {
    // Call a helper function to perform post-order traversal and delete nodes
    destroyTree(root);
}

// Helper function to destroy the tree in post-order
void SpaceSectorBST::destroyTree(Sector* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);

        delete node;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string& filename) {
    // TODO: read the sectors from the input file and insert them into the BST sector map

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int x, y, z;
    char comma;
    std::string line;
    std::getline(file, line);

    while (file >> x >>comma>> y >> comma>> z) {

        insertSectorByCoordinates(x, y, z);
    }
}
Sector* SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z, Sector* currentNode, Sector* parent) {
    if (currentNode == nullptr) {
        Sector* newSector = new Sector(x, y, z);
        newSector->parent = parent;
        return newSector;
    }

    if (x < currentNode->x || (x == currentNode->x && y < currentNode->y) ||
        (x == currentNode->x && y == currentNode->y && z < currentNode->z)) {
        currentNode->left = insertSectorByCoordinates(x, y, z, currentNode->left, currentNode);
    } else {
        currentNode->right = insertSectorByCoordinates(x, y, z, currentNode->right, currentNode);
    }

    return currentNode;
}

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    root = insertSectorByCoordinates(x, y, z, root, nullptr);
}


void SpaceSectorBST::deleteSector(const std::string& sector_code) {
    Sector* nodeToDelete = findSectorByCode(sector_code);

    if (nodeToDelete == nullptr) {
        std::cerr << "Error: Sector with code " << sector_code << " not found." << std::endl;
        return;
    }

    // Case 1: Node with no children (leaf node)
    if (nodeToDelete->left == nullptr && nodeToDelete->right == nullptr) {
        deleteLeafNode(nodeToDelete);
    }
        // Case 2: Node with one child
    else if (nodeToDelete->left == nullptr || nodeToDelete->right == nullptr) {
        deleteNodeWithOneChild(nodeToDelete);
    }
        // Case 3: Node with two children
    else {
        deleteNodeWithTwoChildren(nodeToDelete);
    }
}

void SpaceSectorBST::deleteLeafNode(Sector* nodeToDelete) {
    if (nodeToDelete->parent == nullptr) {
        root = nullptr;
    } else {
        if (nodeToDelete == nodeToDelete->parent->left) {
            nodeToDelete->parent->left = nullptr;
        } else {
            nodeToDelete->parent->right = nullptr;
        }
    }

    delete nodeToDelete;
}

void SpaceSectorBST::deleteNodeWithOneChild(Sector* nodeToDelete) {
    Sector* child = (nodeToDelete->left != nullptr) ? nodeToDelete->left : nodeToDelete->right;

    if (nodeToDelete->parent == nullptr) {
        root = child;
    } else {
        if (nodeToDelete == nodeToDelete->parent->left) {
            nodeToDelete->parent->left = child;
        } else {
            nodeToDelete->parent->right = child;
        }
    }

    child->parent = nodeToDelete->parent;

    delete nodeToDelete;
}

void SpaceSectorBST::deleteNodeWithTwoChildren(Sector* nodeToDelete) {
    Sector* successor = findMinNode(nodeToDelete->right);

    std::swap(nodeToDelete->x, successor->x);
    std::swap(nodeToDelete->y, successor->y);
    std::swap(nodeToDelete->z, successor->z);
    std::swap(nodeToDelete->distance_from_earth, successor->distance_from_earth);
    std::swap(nodeToDelete->sector_code, successor->sector_code);

    if (successor->left != nullptr || successor->right != nullptr) {
        deleteNodeWithOneChild(successor);
    } else {
        deleteLeafNode(successor);
    }
}

Sector* SpaceSectorBST::findMinNode(Sector* startNode) const {
    Sector* current = startNode;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}



void SpaceSectorBST::displaySectorsInOrder() {
    std::cout<<"Space sectors inorder traversal: "<<std::endl;
    displaySectorsInOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorBST::displaySectorsInOrderRecursive(Sector* node) {
    if (node != nullptr) {
        displaySectorsInOrderRecursive(node->left);
        std::cout << node->sector_code << std::endl;
        displaySectorsInOrderRecursive(node->right);
    }
}

void SpaceSectorBST::displaySectorsPreOrder() {
    std::cout<<"Space sectors preorder traversal: "<<std::endl;
    displaySectorsPreOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorBST::displaySectorsPreOrderRecursive(Sector* node) {
    if (node != nullptr) {
        std::cout << node->sector_code << std::endl;
        displaySectorsPreOrderRecursive(node->left);
        displaySectorsPreOrderRecursive(node->right);
    }
}

void SpaceSectorBST::displaySectorsPostOrder() {
    std::cout<<"Space sectors postorder traversal: "<<std::endl;

    displaySectorsPostOrderRecursive(root);
    std::cout << std::endl;
}

void SpaceSectorBST::displaySectorsPostOrderRecursive(Sector* node) {
    if (node != nullptr) {
        displaySectorsPostOrderRecursive(node->left);
        displaySectorsPostOrderRecursive(node->right);
        std::cout << node->sector_code << std::endl;
    }
}
std::vector<Sector*> SpaceSectorBST::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;

    Sector* destination = findSectorByCode(sector_code);

    if (destination == nullptr) {
        return path;
    }

    // Traverse the path from the destination to the root (Earth)
    while (destination != nullptr) {
        path.insert(path.begin(), destination);
        destination = destination->parent;
    }

    return path;
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector*>& path) {
    if (path.empty()) {
        std::cout << "A path to Dr. Elara could not be found." << std::endl;
        return;
    }

    std::cout << "The stellar path to Dr. Elara: ";

    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i]->sector_code;

        if (i < path.size() - 1) {
            std::cout << "->";
        }
    }

    std::cout << std::endl;
}


Sector* SpaceSectorBST::findSectorByCode(const std::string& sector_code) const {
    return findSectorByCodeBFS(root, sector_code);
}

Sector* SpaceSectorBST::findSectorByCodeBFS(Sector* startNode, const std::string& sector_code) const {
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

    return nullptr;  // Sector with the given code not found
}
