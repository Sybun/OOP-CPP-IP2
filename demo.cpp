#include "IntervalTree.h"
#include <iostream>

using namespace itree;

int main() {
    std::cout << "========================================\n";
    std::cout << "IntervalTree Demonstration\n";
    std::cout << "========================================\n\n";

    // Create an IntervalTree
    IntervalTree tree;
    std::cout << "1. Created empty IntervalTree\n";
    std::cout << "   Empty? " << (tree.empty() ? "Yes" : "No") << "\n";
    std::cout << "   Size: " << tree.size() << "\n\n";

    // INSERT operations using insert() and operator+=
    std::cout << "2. Inserting intervals: [1,3], [2,5], [6,8], [4,7]\n";
    tree.insert(1, 3);
    tree.insert(2, 5);
    tree.insert(6, 8);
    tree.insert(4, 7);
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // INSERT using operator+=
    std::cout << "3. Adding interval [9,12] using operator+= \n";
    tree += Interval{9, 12};
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // READ operations using overlaps() and operator[]
    std::cout << "4. Reading: Query for overlaps with [3,6]\n";
    Interval result;
    if (tree.overlaps(3, 6, result)) {
        std::cout << "   Found overlapping interval: [" << result.low << ", " 
                  << result.high << "]\n";
    }
    std::cout << "   Total overlaps with [3,6]: " << tree[Interval{3, 6}] << "\n\n";

    // UPDATE operations using update() and operator%=
    std::cout << "5. Updating interval [1,3] -> [0,2]\n";
    tree.update(1, 3, 0, 2);
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // UPDATE using operator%=
    std::cout << "6. Updating interval [6,8] -> [15,18] using operator%=\n";
    tree %= std::make_pair(Interval{6, 8}, Interval{15, 18});
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // COMPARISON operators
    std::cout << "7. Comparison operations\n";
    IntervalTree tree2;
    tree2 += Interval{10, 15};
    tree2 += Interval{20, 25};
    std::cout << "   tree.size() = " << tree.size() 
              << ", tree2.size() = " << tree2.size() << "\n";
    std::cout << "   tree == tree2? " << (tree == tree2 ? "Yes" : "No") << "\n";
    std::cout << "   tree != tree2? " << (tree != tree2 ? "Yes" : "No") << "\n";
    std::cout << "   tree > tree2?  " << (tree > tree2 ? "Yes" : "No") << "\n";
    std::cout << "   tree < tree2?  " << (tree < tree2 ? "Yes" : "No") << "\n";
    std::cout << "   tree >= tree2? " << (tree >= tree2 ? "Yes" : "No") << "\n";
    std::cout << "   tree <= tree2? " << (tree <= tree2 ? "Yes" : "No") << "\n\n";

    // COPY operations
    std::cout << "8. Deep copy test\n";
    IntervalTree tree3 = tree;
    std::cout << "   tree3 = tree (copy constructor)\n";
    std::cout << "   tree3.size() = " << tree3.size() << "\n";
    std::cout << tree3.toString();

    // DELETE operations using remove() and operator-=
    std::cout << "9. Deleting interval [2,5] using remove()\n";
    tree.remove(2, 5);
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // DELETE using operator-=
    std::cout << "10. Deleting interval [4,7] using operator-=\n";
    tree -= Interval{4, 7};
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << tree.toString();

    // CLEAR operation using operator!
    std::cout << "11. Clearing tree using operator!\n";
    !tree;
    std::cout << "   Size: " << tree.size() << "\n";
    std::cout << "   Empty? " << (tree.empty() ? "Yes" : "No") << "\n";
    std::cout << tree.toString();

    // Demonstration of exception handling
    std::cout << "12. Exception handling demonstration\n";
    try {
        std::cout << "   Attempting to insert invalid interval [5, 3]...\n";
        tree.insert(5, 3);
    } catch (const IntervalException& e) {
        std::cout << "   Caught IntervalException: " << e.what() << "\n";
    }

    try {
        std::cout << "   Attempting to remove non-existent interval [100, 200]...\n";
        tree.remove(100, 200);
    } catch (const std::invalid_argument& e) {
        std::cout << "   Caught std::invalid_argument: " << e.what() << "\n";
    }

    try {
        std::cout << "   Attempting to insert duplicate interval [10,15]...\n";
        tree += Interval{10, 15};
        tree += Interval{10, 15};
    } catch (const IntervalException& e) {
        std::cout << "   Caught IntervalException: " << e.what() << "\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "Demonstration Complete\n";
    std::cout << "========================================\n";

    return 0;
}
