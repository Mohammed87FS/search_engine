// Example C++ code demonstrating modern patterns

#include <vector>
#include <memory>
#include <algorithm>

class Resource {
    // RAII: Resource managed by object lifetime
public:
    Resource() { /* acquire resource */ }
    ~Resource() { /* release resource */ }
    
    // Move semantics: transfer ownership
    Resource(Resource&& other) noexcept = default;
    Resource& operator=(Resource&& other) noexcept = default;
    
    // Delete copy operations
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
};

void demonstrate_stl() {
    std::vector<int> numbers = {3, 1, 4, 1, 5};
    std::sort(numbers.begin(), numbers.end());
    
    auto it = std::find_if(numbers.begin(), numbers.end(),
        [](int n) { return n > 3; });
}
