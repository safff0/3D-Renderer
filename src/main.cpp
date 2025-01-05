#include <glm/glm.hpp>

#include <iostream>

int main() {
    std::cout << glm::dot(glm::vec3{1, 2, 3}, glm::vec3{1, 2, 3}) << "\n";
    return 0;
}