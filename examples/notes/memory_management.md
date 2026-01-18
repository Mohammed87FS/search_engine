# Memory Management in C++

## RAII Pattern
RAII ensures that resources are automatically released when objects go out of scope.
This prevents memory leaks and makes code more robust.

## Smart Pointers
- `std::unique_ptr`: Exclusive ownership, cannot be copied
- `std::shared_ptr`: Shared ownership with reference counting
- `std::weak_ptr`: Non-owning reference to shared_ptr

## Move Semantics
Move semantics allow transferring ownership without expensive copies.
Use `std::move()` to indicate that an object can be moved from.
