#include "SpaceSectorBST.h"
#include "SpaceSectorLLRBT.h"
#include "kNN_DAT_Parser.h"
#include "kNN.h"
#include "KD_Tree.h"
#include "KDT_Node.h"
#include "kNN_Data.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char**argv) {

    //************ Task 1: Space Sector Mapping ***********//
    SpaceSectorBST sector_map_BST;
    sector_map_BST.readSectorsFromFile(argv[1]);
    sector_map_BST.displaySectorsInOrder();
    sector_map_BST.displaySectorsPreOrder();
    sector_map_BST.displaySectorsPostOrder();

    std::vector<Sector*> stellar_path = sector_map_BST.getStellarPath("45RDF");
    sector_map_BST.printStellarPath(stellar_path);

    cout << endl;

    stellar_path = sector_map_BST.getStellarPath("31SUF");
    sector_map_BST.printStellarPath(stellar_path);

    cout << endl;
    //************ Task 2: Optimizing Space Sector Mapping ***********//
    SpaceSectorLLRBT sector_map_LLRBT;
    sector_map_LLRBT.readSectorsFromFile(argv[1]);
    sector_map_LLRBT.displaySectorsInOrder();
    sector_map_LLRBT.displaySectorsPreOrder();
    sector_map_LLRBT.displaySectorsPostOrder();

    std::vector<Sector*> stellar_path_2 = sector_map_LLRBT.getStellarPath("31SUF");
    sector_map_LLRBT.printStellarPath(stellar_path_2);

    cout << endl;

    stellar_path_2 = sector_map_LLRBT.getStellarPath("45RDF");
    sector_map_LLRBT.printStellarPath(stellar_path_2);

    cout << endl;
    //************ BONUS Task 3: Planetary Classification System (for extra 20 pts.) ***********//

    // get the filename from the 2nd command line arg
    std::string trainingDataFileName = argv[2];

    // Example:
    // Create an instance of the kNN_Dat_Parser
    kNN_Dat_Parser parser;
    // Parse the dataset
    Dataset data = parser.parse(trainingDataFileName);
    int k = 2;
    KNN knnClassifier(k, data.threshold);
    knnClassifier.train(data);
    // get the tree
    KD_Tree tree = knnClassifier.tree;
    // test instance
    std::vector<double> testFeatures = {1.3232202558909871, 3.172770177200133, 18.57605415623152, 0.22339666018454057, 81.15184706218832, 695.926406517461, 0.9139624956958795};
    Point testPoint(testFeatures, "");
    // predict the label
    int predictedLabel = knnClassifier.predict(testPoint); // Expected value = 0
    return 0;
}


