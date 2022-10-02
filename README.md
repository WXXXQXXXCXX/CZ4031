# CZ4031
CZ4031 projects

#### Code Structure
```
.
├── bptree                   # implementation of b+tree
├── storage                  # implementation of storage component
├── exp                      # code for experiments, and complete outputs for the experiments.
├── CMakeLists.txt           
├── data.tsv                 
└── README.md
```

To run the experiments, comment or uncomment any function in `exp/experiments.cpp`

```c
//Experiment 1&2
insert(&tree, &mem, file_path);

//Experiment 3
find(500, &tree, &mem);

//Experiment 4
range(30000,40000,&tree, &mem);

//Experiment 5
del(1000, &tree, &mem);
//delete_range(100,200, &tree, &mem);
//tree.printTree();
```

Edit the degree of b+tree in `bptree/bptree.h`, set `MAX=16` for 200B block, `MAX=41` for 500B block.<br><br>

Some util functions to check tree content and storage content:
```c
// print tree structure
void BPTree::printTree();

// root, content of first child, and node count
int BPTree::getTreeInfo();

//get size of the storage
unsigned int Memory::get_mem_used();
```

#### Installation
 ```sh
 
#run cmake
cmake -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S /path/to/CZ4031 -B /path/to/CZ4031/cmake-build-debug

# build
cmake --build /path/to/CZ4031/cmake-build-debug --target CZ4031 -j 3

# run
# argv[1]: block size
# argv[2]: /path/to/CZ4031/data.tsv
/path/to/CZ4031/cmake-build-debug/CZ4031 argv[1] argv[2]
```
