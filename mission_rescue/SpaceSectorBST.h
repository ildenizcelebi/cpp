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
    void clearTree(Sector *node);
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void deleteSector(const std::string& sector_code);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);

    Sector* insert_recursive(Sector* node, int x, int y, int z);
    Sector* delete_recursive(Sector* node, const std::string &sector_code);
    Sector* findSectorByCode(Sector* node, const std::string &target_code);
    void inOrder_recursive(Sector* node);
    void preOrder_recursive(Sector* node);
    void postOrder_recursive(Sector* node);
    void getStellarPath_recursive(Sector* current, Sector* destination, std::vector<Sector *> &path);

    void printTreeStructure();
    void printTreeStructure(Sector* node, int indent);



};
#endif // SPACESECTORBST_H