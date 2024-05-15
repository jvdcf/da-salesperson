As part of the second project of Algorithm Design, the goal was to implement various algorithms to solve the 
Travelling Salesman Problem (TSP), using backtracking and various heuristics and approximations.

This project was carried out by students from group 163: 
 - Maria Rabelo (up202000130@up.pt)
 - Guilherme Matos (up202208755@up.pt)
 - João Ferreira (up202208393@up.pt)

---

# Running the project

The project was developed in C++ and uses the CMake tool to compile the source code and this documentation.  

This program takes as input:
- `edges.csv`: A path to a csv file containing the edges of the graph.
- `[nodes.csv]`: An optional path to a csv file containing the nodes of the graph, and their respective coordinates.

> **Warning:** If a `nodes.csv` file is not provided, the algorithms that generate new edges using the nodes' coordinates 
> will not be available.

Make sure that the csv files are in the correct format and that the paths are correct.

### Using the shell script (Linux only)
1. Make sure that the C / C++ dependencies are installed on your system.
2. Execute the script `run.sh` (located in the directory of the project) in the terminal, giving the correct paths as 
   arguments.
   For example:
   ```bash
   ./run.sh dataset/Real-world\ Graphs/graph1/edges.csv dataset/Real-world\ Graphs/graph1/nodes.csv
   ```

> **Warning:** Make sure the script has the correct permissions to be executed.  

> **Note:** Feel free to change the script to suit your needs.

### Using the terminal

```
cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
make -j$(nproc)
./DA2324_PRJ2_G163 <edges.csv> [<nodes.csv>]
```

> **Warning:** Don't forget to **change the arguments to the correct paths**.

### Using CLion

1. Open the project folder in CLion.
2. Create a CMake profile with the build type set to `Release`.
3. Select one of the configurations already created.  
   _or_  
   Create a new configuration with the correct program arguments and working directory.  
   For example:
    - Program arguments: `dataset/Toy-Graphs/tourism.csv`
    - Working directory: `$ProjectFileDir$`
4. Run the configuration.

---

# Notes

- Certain commands may require extended execution time in `Debug` build mode. 
  Ensure the build mode is set to `Release` prior to running the program. 
- The integrated terminal in CLion may exhibit different behavior compared to the system terminal.
  It is recommended to use the system terminal for running the program when feasible.
