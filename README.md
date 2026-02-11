# LFA Laboratory Work Repository

This repository contains my laboratory work for the Formal Languages & Finite Automata (LFA) course.

## Repository Structure

```
LFA sem 4/
│
├── Lab1/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── include/
│   │   ├── finiteAutomaton.cpp
│   │   ├── finiteAutomaton.h
│   │   ├── grammar.cpp
│   │   └── grammar.h
│   └── build/           # (created automatically, not tracked in git)
│
├── README.md            # (this file)
└── ...                  # (future labs will follow a similar structure)
```

- Each lab is in its own folder (e.g., `Lab1/`, `Lab2/`, ...).
- Source code and headers are inside each lab's folder.
- Each lab contains its own `CMakeLists.txt` for building.
- The `build/` directory is for CMake build files and is not tracked by git.

## How to Compile and Run a Lab

1. **Open a terminal and navigate to the lab folder:**
   ```
   cd "LFA sem 4/Lab1"
   ```
2. **Create a build directory (if it doesn't exist):**
   ```
   mkdir -p build
   cd build
   ```
3. **Generate build files with CMake:**
   ```
   cmake ..
   ```
4. **Build the project:**
   ```
   cmake --build .
   ```
   Or, if using Makefiles:
   ```
   make
   ```
5. **Run the program:**
   ```
   ./test
   ```
   Or, with the custom run target (if available):
   ```
   cmake --build . --target run
   # or
   make run
   ```

## Notes
- Repeat the above steps for each lab, changing to the appropriate lab folder (e.g., `Lab2/`, `Lab3/`, etc.).
- All labs are designed to be cross-platform using CMake.
- The `build/` directory can be safely deleted and recreated at any time.

---

**Author:** Gabi (based on the LFA course by Cretu Dumitru, Vasile Drumea, and Irina Cojuhari)
