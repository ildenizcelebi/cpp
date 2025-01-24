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
    void clearTree(Sector *node);
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);

    Sector* insert_recursive(Sector *node, int x, int y, int z);

    Sector *rotateRight(Sector *node);
    Sector *rotateLeft(Sector *node);
    void flipColors(Sector *node);
    bool isRed(Sector *node);

    void inOrder_recursive(Sector* node);
    void preOrder_recursive(Sector *node);
    void postOrder_recursive(Sector *node);

    void printTreeStructure();
    void printTreeStructure(Sector* node, int indent);

    Sector *findSectorByCode(Sector *node, const std::string &target_code);

    void pathToEarth(Sector *node, Sector *current, std::vector<Sector *> &path);

    void pathToDestination(Sector *node, Sector *destination, std::vector<Sector *> &path);

    Sector *findLastCommonNode(const std::vector<Sector *> &earth_path, const std::vector<Sector *> &destination_path);
};

#endif // SPACESECTORLLRBT_H
