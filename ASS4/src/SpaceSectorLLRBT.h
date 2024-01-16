#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

class SpaceSectorLLRBT {
public:
    Sector* root;
    SpaceSectorLLRBT();
    ~SpaceSectorLLRBT();
    void destroyTree(Sector* node) ;
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    void displaySectorsPostOrderRecursive(Sector* node) ;
    void displaySectorsPreOrderRecursive(Sector* node) ;
    void displaySectorsInOrderRecursive(Sector* node) ;
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector* insertSectorByCoordinates(Sector* currentNode, int x, int y, int z, Sector* parent) ;
    bool isRed(Sector* node) const ;
    Sector* rotateLeft(Sector* h) ;
    Sector* rotateRight(Sector* h) ;
    void flipColors(Sector* node) ;
    bool getPath(Sector* root, std::vector<std::string>& arr, std::string x);
    Sector* findSectorByCode(const std::string& sector_code) const ;
    Sector* findSectorByCodeBFS(Sector* startNode, const std::string& sector_code) const;
    Sector* fixViolations(Sector* node) ;

};

#endif // SPACESECTORLLRBT_H
