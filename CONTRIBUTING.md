# Styleguide

## Includes
Imports and uses have to be split in three groups (the order matters!):
1. includes from `std`
2. includes from external libraries
3. icludes from `tsl`

Example:
```
// First include group containing includes from the stdlib
#include <vector>

// Second include group containing includes from external libs, in this case glm
#include <glm/glm.hpp>

// Third include group containing includes from the tsl
#inlcude 'line.hpp'


// First use group containing uses from the stdlib
using std::vector;

// Second use group containing uses from external libs, in this case glm
using glm::vec3;
```

---
No transitive includes are allowed! In other words: If an external type is used in the `.hpp` and the `.cpp` you need to include in twice, once per file!

---

You have to use include guards in `.hpp` files, `#pragma once` is not allowed.
