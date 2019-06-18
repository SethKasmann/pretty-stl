# pretty-stl
C++ header only library to print stl containers.

### Purpose
The idea is to be able to copy all iteratable containers from the c++ standard template library to the output stream. This includes the following containers:

#### Sequence containers
* std::array
* std::vector
* std::deque
* std::forward_list
* std::list
#### Associative containers
* std::set
* std::map
* std::multiset
* std::multimap
#### Unordered associative containers
* std::unordered_set
* std::unordered_map
* std::unordered_multiset
* std::unordered_multimap

### Example
``` c++
#include <iostream>
#include <vector>
#include <map>
#include "pretty_stl.h"

int main() {
  // Create a vector containing integers.
  std::vector<int> my_vector{1, 2, 3, 4, 5};
  // Print the vector.
  std::cout << "my_vector " << my_vector << '\n';

  // Create a map of int to string.
  std::map<int, std::string> my_map{{1, "Hello"}, {2, "world!"}};
  // Print the map.
  std::cout << "my_map " << my_map << '\n';

  return 0;
}
```

### Output
```
my_vector [1, 2, 3, 4, 5]
my_map {1: Hello, 2: world!}
```
