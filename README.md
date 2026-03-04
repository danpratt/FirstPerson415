# Procedural Terrain & Foliage Generator

**Developer:** Daniel R. Pratt  
**Course:** GAM-495 (Game Programming Capstone)  
**Platform:** Unreal Engine 5  
**Language:** C++ / Blueprints  

## Project Overview
This repository contains a C++ based procedural generation tool built for Unreal Engine. The project dynamically generates a 3D terrain mesh using Perlin noise and populates it with complex, multi-species biomes using highly optimized Hierarchical Instanced Static Mesh (HISM) components. 

The system allows for real-time manipulation of the terrain's ecosystem through a custom UMG user interface, demonstrating a complete pipeline from back-end procedural math to front-end player control.

## Key Technical Features

### 1. Procedural Terrain Generation
* **Dynamic Mesh Construction:** Utilizes the `UProceduralMeshComponent` to construct a landscape from scratch at runtime.
* **Perlin Noise Algorithms:** Calculates vertex Z-heights using 2D Perlin noise to create natural-looking hills, valleys, and ridges.

### 2. Rule-Based Biome Spawning
* **Struct-Driven Architecture:** Employs a custom `FFoliageSetup` C++ struct, allowing designers to easily define unique rules for dozens of distinct foliage types (trees, grass, ferns) directly within the Unreal editor.
* **Elevation Masking:** Evaluates vertex Z-coordinates against defined minimum and maximum thresholds to prevent foliage from spawning underwater or above snowlines.
* **Slope Detection (Vector Math):** Calculates the dot product of the terrain's vertex normal and the world's Up Vector `(0, 0, 1)`. If the resulting angle exceeds the defined slope threshold, foliage generation is aborted to prevent trees from growing horizontally on cliff faces.

### 3. High-Performance Rendering
* **Dynamic HISM Allocation:** To handle the massive vertex overhead of rendering thousands of meshes, the system dynamically instantiates dedicated `UHierarchicalInstancedStaticMeshComponent` objects for each foliage type in the array.
* **Collision Optimization:** Automatically disables collision overhead (`ECollisionEnabled::NoCollision`) on generated instances to maintain high frame rates and allow seamless player navigation.

### 4. Interactive UI Control
* **UMG Integration:** Features a screen-space UI slider bound to a global density multiplier. 
* **Event-Driven Execution:** To prevent frame-drops during manipulation, the logic utilizes an `On Mouse Capture End` event to trigger the `GenerateFoliage()` C++ rebuild only after the user has finalized their selection. 

## Code Highlights
The core logic for the procedural evaluation and HISM spawning can be found within the `GenerateFoliage()` function of the `PerlinProcTerrain.cpp` class.
