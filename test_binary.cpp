#include <iostream>

int main() {
    int code = 4;
    int len = 2;
    
    std::cout << "Code " << code << " with length " << len << " bits: ";
    for (int b = len - 1; b >= 0; b--) {
        std::cout << ((code >> b) & 1);
    }
    std::cout << std::endl;
    
    std::cout << "Full binary of 4: ";
    for (int b = 7; b >= 0; b--) {
        std::cout << ((code >> b) & 1);
    }
    std::cout << std::endl;
    
    return 0;
}