#ifndef SPACESECTORBST_H
#define SPACESECTORBST_H

#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

#include "Sector.h"

class SpaceSectorBST {
  
public:
    Sector *root;
    SpaceSectorBST();
    ~SpaceSectorBST();
    void destroyTree(Sector* node) ;
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void deleteSector(const std::string& sector_code);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector* insertSectorByCoordinates(int x, int y, int z, Sector* currentNode, Sector* parent) ;
    void displaySectorsInOrderRecursive(Sector* node) ;
    void displaySectorsPreOrderRecursive(Sector* node) ;
    void displaySectorsPostOrderRecursive(Sector* node) ;
    Sector* findSectorByCode(const std::string& sector_code) const ;
    Sector* findSectorByCodeBFS(Sector* startNode, const std::string& sector_code) const;
    Sector* findMinNode(Sector* startNode) const ;
    void deleteNodeWithTwoChildren(Sector* nodeToDelete) ;
    void deleteNodeWithOneChild(Sector* nodeToDelete);
    void deleteLeafNode(Sector* nodeToDelete) ;
};

#endif // SPACESECTORBST_H
