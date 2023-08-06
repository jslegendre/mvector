# mvector
Clang compatible file-backed std::vectors 

## Overview
mvector is a C++ library that provides a file-backed version of std::vector that is compatible with both GCC and Clang. It allows you to save and restore vectors from disk, making it convenient for situations where you need persistent data storage.

## Motivation
The motivation behind creating `mvector` was the need for a file-backed vector implementation that could work with Clang. Existing open-source implementations either only supported GCC or were complete re-implementations of std::vector, lacking the compatibility and ease of use that comes with the standard library.

## Features
- mvector inherits all the features and functionalities of the standard std::vector, allowing seamless integration into existing codebases.
- Automatically creates the file backing if it does not exist, simplifying setup and usage.
- Cross-compiler compatibility.
- C++11 -> C++20

## Usage
To use mvector, simply include the appropriate header file and create an instance of the vector with the desired type and file path:
```
#include "mvector/mvector.hpp"

int main() {
    mvector<int> v("/path/to/file/backing");
    // Now, 'v' can be used like any other vector.
    return 0;
}
```

## Important Notes
- All files are mapped as read-write.
- If the file backing does not exist, mvector will create it automatically.
- mvector is designed to work with value types only.
