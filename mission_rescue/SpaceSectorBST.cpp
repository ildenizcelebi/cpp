#include "SpaceSectorBST.h"
#include <iomanip> // for std::setw

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    // Free any dynamically allocated memory in this class.
    clearTree(root);
}
void SpaceSectorBST::clearTree(Sector* node) {
    if (node != nullptr) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
    root = nullptr;
}

void SpaceSectorBST::readSectorsFromFile(const std::string& filename) {
    // TODO: read the sectors from the input file and insert them into the BST sector map
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

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    // Instantiate and insert a new sector into the space sector BST map according to the
    // coordinates-based comparison criteria.
    root = insert_recursive(root, x, y, z);
}

Sector* SpaceSectorBST::insert_recursive(Sector* node, int x, int y, int z) {
    if (node == nullptr) {
        return new Sector(x, y, z);
    }
    if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) {
        node->left = insert_recursive(node->left, x, y, z);
    } else {
        node->right = insert_recursive(node->right, x, y, z);
    }
    return node;
}

void SpaceSectorBST::deleteSector(const std::string& sector_code) {
    // TODO: Delete the sector given by its sector_code from the BST.
    Sector* target = findSectorByCode(root, sector_code);
    if (target != nullptr) {
        root = delete_recursive(root, sector_code);
    }
}
Sector* SpaceSectorBST::delete_recursive(Sector* node, const std::string& sector_code) {
    if (node == nullptr) {
        return node;
    }
    Sector* target = findSectorByCode(root, sector_code);
    if (target->x < node->x || (target->x == node->x && target->y < node->y) ||
        (target->x == node->x && target->y == node->y && target->z < node->z)) { //left
        node->left = delete_recursive(node->left, sector_code);
    } else if (target->x > node->x || (target->x == node->x && target->y > node->y) ||
               (target->x == node->x && target->y == node->y && target->z > node->z)) { //right
        node->right = delete_recursive(node->right, sector_code);
    } else {
        if (node->left == nullptr) {
            Sector* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            Sector* temp = node->left;
            delete node;
            return temp;
        }
        Sector* successorParent = node;
        Sector* successor = node->right;
        while (successor->left != nullptr) {
            successorParent = successor;
            successor = successor->left;
        }
        if (successorParent != node) {
            successorParent->left = successor->right;
        } else {
            successorParent->right = successor->right;
        }
        node->sector_code = successor->sector_code;
        node->x = successor->x;
        node->y = successor->y;
        node->z = successor->z;
        node->distance_from_earth = successor->distance_from_earth;

        delete successor;
    }
    return node;
}

Sector* SpaceSectorBST::findSectorByCode(Sector* node, const std::string& target_code) {
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

void SpaceSectorBST::displaySectorsInOrder() {
    // TODO: Traverse the space sector BST map in-order and print the sectors
    // to STDOUT in the given format.
    cout << "Space sectors inorder traversal:" << endl;
    inOrder_recursive(root);
    cout << endl;
}
void SpaceSectorBST::inOrder_recursive(Sector* node) {
    if (node == nullptr) {
        return;
    }
    inOrder_recursive(node->left);
    cout << node->sector_code << endl;
    inOrder_recursive(node->right);
}

void SpaceSectorBST::displaySectorsPreOrder() {
    // TODO: Traverse the space sector BST map in pre-order traversal and print
    // the sectors to STDOUT in the given format.
    cout << "Space sectors preorder traversal:" << endl;
    preOrder_recursive(root);
    cout << endl;
}

void SpaceSectorBST::preOrder_recursive(Sector* node) {
    if (node == nullptr) {
        return;
    }
    cout << node->sector_code << endl;
    preOrder_recursive(node->left);
    preOrder_recursive(node->right);
}

void SpaceSectorBST::displaySectorsPostOrder() {
    // TODO: Traverse the space sector BST map in post-order traversal and print
    // the sectors to STDOUT in the given format.
    cout << "Space sectors postorder traversal:" << endl;
    postOrder_recursive(root);
    cout << endl;
}

void SpaceSectorBST::postOrder_recursive(Sector *node) {
    if (node == nullptr) {
        return;
    }
    postOrder_recursive(node->left);
    postOrder_recursive(node->right);
    cout << node->sector_code << endl;
}

std::vector<Sector*> SpaceSectorBST::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!
    Sector* destination = findSectorByCode(root, sector_code);
    if (destination != nullptr) {
        getStellarPath_recursive(root, destination, path);
        path.push_back(destination);
    }
    return path;
}

void SpaceSectorBST::getStellarPath_recursive(Sector* current, Sector* destination, std::vector<Sector*>& path) {
    if (current == nullptr || current->sector_code == destination->sector_code) {
        return;
    }

    path.push_back(current);

    if (destination->x < current->x || (destination->x == current->x && destination->y < current->y) ||
        (destination->x == current->x && destination->y == current->y && destination->z < current->z)) {
        getStellarPath_recursive(current->left, destination, path);
    } else {
        getStellarPath_recursive(current->right, destination, path);
    }
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector*>& path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function
    // to STDOUT in the given format.
    if (!path.empty()) {
        cout << "The stellar path to Dr. Elara: ";
        int count = 0;
        for (Sector* sector : path){
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

void SpaceSectorBST::printTreeStructure() {
    printTreeStructure(root, 0);
}

void SpaceSectorBST::printTreeStructure(Sector* node, int indent) {
    if (node != nullptr) {
        printTreeStructure(node->right, indent + 4);

        cout << setw(indent) << " ";
        cout << node->sector_code << endl;

        printTreeStructure(node->left, indent + 4);
    }
}