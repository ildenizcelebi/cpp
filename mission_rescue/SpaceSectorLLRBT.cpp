#include <iomanip>
#include <algorithm>
#include "SpaceSectorLLRBT.h"

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string& filename) {
    // TODO: read the sectors from the input file and insert them into the LLRBT sector map
    // according to the given comparison critera based on the sector coordinates.
    ifstream inputFile(filename);
    if (inputFile.is_open()) {
        string line;
        getline(inputFile, line);
        while (getline(inputFile, line)) {
            istringstream iss(line);
            string str_x, str_y, str_z;
            if (getline(iss, str_x, ',') && getline(iss, str_y, ',') && getline(iss, str_z, ',')) {
                int x = stoi(str_x);
                int y = stoi(str_y);
                int z = stoi(str_z);

                insertSectorByCoordinates(x, y, z);
            }
        }
        inputFile.close();
    } else {
        cerr << "Unable to open file: " << filename << std::endl;
    }
}

// Remember to handle memory deallocation properly in the destructor.
SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    // TODO: Free any dynamically allocated memory in this class.
    clearTree(root);

}
void SpaceSectorLLRBT::clearTree(Sector* node) {
    if (node != nullptr) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
    root = nullptr;
}

void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    // TODO: Instantiate and insert a new sector into the space sector LLRBT map
    // according to the coordinates-based comparison criteria.
    root = insert_recursive(root, x, y, z);
    root->color = BLACK;
}
Sector* SpaceSectorLLRBT::insert_recursive(Sector* node, int x, int y, int z) {
    if (node == nullptr) {
        return new Sector(x, y, z);
    }

    if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) {
        node->left = insert_recursive(node->left, x, y, z);
    } else if (x > node->x || (x == node->x && y > node->y) || (x == node->x && y == node->y && z > node->z)) {
        node->right = insert_recursive(node->right, x, y, z);
    } else {
        return node;
    }

    if (isRed(node->right) && !isRed(node->left)) {
        node = rotateLeft(node);
    }
    if (isRed(node->left) && isRed(node->left->left)) {
        node = rotateRight(+node);
    }
    if (isRed(node->left) && isRed(node->right)) {
        flipColors(node);
    }

    return node;
}

bool SpaceSectorLLRBT::isRed(Sector* node) {
    if (node == nullptr) {
        return false;
    }
    return node->color == RED;
}

void SpaceSectorLLRBT::flipColors(Sector* node) {
    node->color = RED;
    node->left->color = BLACK;
    node->right->color = BLACK;
}

Sector* SpaceSectorLLRBT::rotateLeft(Sector* node) {
    Sector* temp = node->right;
    node->right = temp->left;
    temp->left = node;
    temp->color = node->color;
    node->color = RED;
    return temp;
}

Sector* SpaceSectorLLRBT::rotateRight(Sector* node) {
    Sector* temp = node->left;
    node->left = temp->right;
    temp->right = node;
    temp->color = node->color;
    node->color = RED;
    return temp;
}

void SpaceSectorLLRBT::displaySectorsInOrder() {
    // TODO: Traverse the space sector LLRBT map in-order and print the sectors
    // to STDOUT in the given format.
    cout << "Space sectors inorder traversal:" << endl;
    inOrder_recursive(root);
    cout << endl;
}
void SpaceSectorLLRBT::inOrder_recursive(Sector* node) {
    if (node == nullptr) {
        return;
    }
    inOrder_recursive(node->left);
    cout << (node->color ? "RED" : "BLACK") << " sector: " << node->sector_code << endl;
    inOrder_recursive(node->right);
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    // TODO: Traverse the space sector LLRBT map in pre-order traversal and print
    // the sectors to STDOUT in the given format.
    cout << "Space sectors preorder traversal:" << endl;
    preOrder_recursive(root);
    cout << endl;
}

void SpaceSectorLLRBT::preOrder_recursive(Sector* node) {
    if (node == nullptr) {
        return;
    }
    cout << (node->color ? "RED" : "BLACK") << " sector: " << node->sector_code << endl;
    preOrder_recursive(node->left);
    preOrder_recursive(node->right);
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    // TODO: Traverse the space sector LLRBT map in post-order traversal and print
    // the sectors to STDOUT in the given format.
    cout << "Space sectors postorder traversal:" << endl;
    postOrder_recursive(root);
    cout << endl;
}

void SpaceSectorLLRBT::postOrder_recursive(Sector *node) {
    if (node == nullptr) {
        return;
    }
    postOrder_recursive(node->left);
    postOrder_recursive(node->right);
    cout << (node->color ? "RED" : "BLACK") << " sector: " << node->sector_code << endl;
}

std::vector<Sector*> SpaceSectorLLRBT::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!
    Sector* destination = findSectorByCode(root, sector_code);
    if (destination != nullptr) {
        Sector* earth = findSectorByCode(root, "0SSS");
        if (earth != nullptr) {
            vector<Sector*> earth_path;
            pathToEarth(root, earth, earth_path);
            vector<Sector*> destination_path;
            pathToDestination(root, destination, destination_path);
            Sector* lastCommon = findLastCommonNode(earth_path, destination_path);
            for (int i = earth_path.size() - 1; i >= 0; --i) {
                Sector* sector = earth_path[i];
                path.push_back(sector);
                if (sector == lastCommon) {
                    break;
                }
            }
            bool foundLastCommon = false;
            for (Sector* sector : destination_path) {
                if (foundLastCommon) {
                    path.push_back(sector);
                }
                if (sector == lastCommon) {
                    foundLastCommon = true;
                }
            }
        } else {
            path.clear();
        }
    } else {
        path.clear();
    }
    return path;
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector*>& path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function
    // to STDOUT in the given format.
    if (!path.empty()) {
        cout << "The stellar path to Dr. Elara: ";
        int count = 0;
        for (Sector *sector: path) {
            count++;
            cout << sector->sector_code;
            if (count < path.size()) {
                cout << "->";
            }
        }
        cout << endl;
    } else {
        cout << "A path to Dr. Elara could not be found." << endl;
    }
}

Sector* SpaceSectorLLRBT::findLastCommonNode(const std::vector<Sector*>& earth_path, const std::vector<Sector*>& destination_path) {
    std::vector<Sector*> commonNodes;

    for (Sector* sector1 : earth_path) {
        for (Sector* sector2: destination_path) {
            if (sector1==sector2) {
                commonNodes.push_back(sector1);
            }
        }
    }
    return commonNodes.back();
}

void SpaceSectorLLRBT::pathToEarth(Sector* node, Sector* current, std::vector<Sector*>& path) {
    if (node == nullptr || current == nullptr) {
        return;
    }
    if (node->sector_code == current->sector_code) {
        path.push_back(node);
        return;
    }
    path.push_back(node);
    if (current->x < node->x || (current->x == node->x && current->y < node->y) || (current->x == node->x && current->y == node->y && current->z < node->z)) {
        pathToEarth(node->left, current, path);
    } else {
        pathToEarth(node->right, current, path);
    }
}

void SpaceSectorLLRBT::pathToDestination(Sector* node, Sector* destination, std::vector<Sector*>& path) {
    if (node == nullptr || destination == nullptr) {
        return;
    }

    if (node->sector_code == destination->sector_code) {
        path.push_back(node);
        return;
    }

    path.push_back(node);

    if (destination->x < node->x || (destination->x == node->x && destination->y < node->y) || (destination->x == node->x && destination->y == node->y && destination->z < node->z)) {
        pathToDestination(node->left, destination, path);
    } else {
        pathToDestination(node->right, destination, path);
    }
}


Sector* SpaceSectorLLRBT::findSectorByCode(Sector* node, const std::string& target_code) {
    if (node == nullptr) {
        return nullptr;
    }
    Sector* leftResult = findSectorByCode(node->left, target_code);
    if (leftResult != nullptr) {
        return leftResult;
    }
    if (node->sector_code == target_code) {
        return node;
    }
    Sector* rightResult = findSectorByCode(node->right, target_code);
    if (rightResult != nullptr) {
        return rightResult;
    }
    return nullptr;
}

void SpaceSectorLLRBT::printTreeStructure() {
    printTreeStructure(root, 0);
}

void SpaceSectorLLRBT::printTreeStructure(Sector* node, int indent) {
    if (node != nullptr) {
        printTreeStructure(node->right, indent + 4);

        cout << setw(indent) << " ";
        cout << node->sector_code << endl;

        printTreeStructure(node->left, indent + 4);
    }
}