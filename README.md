# Procedural Ocean Rendering (OpenGL)

A real-time ocean simulation using **procedural wave synthesis**, **domain warping**, and **analytical normal computation**, implemented in modern OpenGL (3.3 Core Profile).

---

## Preview

- 🎥 [Ocean Simulation Clip 1](preview/preview1.mp4)  
- 🎥 [Ocean Simulation Clip 2](preview/preview2.mp4)

---

## Features

-  Multi-layered procedural wave system  
-  Domain warping for natural turbulence  
-  Analytical normal reconstruction (no normal maps)  
-  Phong-based lighting with horizon attenuation  
-  Fully GPU-driven surface deformation  
-  High-resolution grid mesh (3000×3000)  

---

## 🧠 Core Idea

The ocean surface is modeled as a **dynamic height field**:

\[
y = f(x, z, t)
\]

Instead of simulating fluid dynamics directly, the surface is generated using a **sum of nonlinear waves**, evaluated in the vertex shader each frame.

---

## Mathematics

### Wave Function

Each wave contributes:

\[
h_i(x, z, t) = A_i \cdot e^{\sin(k_i (\mathbf{d}_i \cdot \mathbf{x}) + \omega_i t)}
\]

Where:

- \( A_i \) → amplitude  
- \( k_i \) → frequency  
- \( \mathbf{d}_i \) → direction  
- \( \omega_i \) → phase speed  

### Why `exp(sin(...))`?

This nonlinearity creates:
- sharper wave crests  
- smoother troughs  
- more natural ocean-like shapes  

---

### Multi-Scale Waves

Each iteration scales:

- amplitude ↓ (`× 0.8`)
- frequency ↑ (`× 1.22`)

This produces a **fractal-like spectrum**:
- large slow waves  
- small fast ripples  

---

### Domain Warping

Instead of sampling waves at the original position:

\[
\mathbf{x}
\]

we iteratively distort the domain:

\[
\mathbf{x}_{i+1} = \mathbf{x}_i + \alpha \nabla h_i(\mathbf{x}_i)
\]

This introduces:
- turbulence  
- chaotic interference  
- non-repeating patterns  

---

## 📉 Surface Normals (Analytical)

Normals are computed from derivatives:

\[
\frac{\partial y}{\partial x}, \quad \frac{\partial y}{\partial z}
\]

Using tangent vectors:

\[
T_x = (1, \partial y / \partial x, 0)
\]
\[
T_z = (0, \partial y / \partial z, 1)
\]

Final normal:

\[
\mathbf{n} = T_z \times T_x
\]

 Accurate  
 Smooth  
 No artifacts from finite differences  

---

## Lighting Model

A **Phong reflection model** is used:

- **Ambient** → base water color  
- **Diffuse** → Lambertian reflection  
- **Specular** → sharp highlights (shininess = 128)  
- **Horizon fade** → reduces reflections at grazing angles  

---

## Architecture

### CPU (C++)

- Window/context: GLFW  
- OpenGL loading: GLAD  
- Math: GLM  
- Mesh generation (grid plane)  
- Shader compilation and uniform uploads  

---

### GPU (Shaders)

#### Vertex Shader
- Computes wave displacement  
- Applies domain warping  
- Calculates analytical normals  

#### Fragment Shader
- Applies lighting model  
- Produces final water color  

---

## Rendering Pipeline

1. Generate grid mesh (CPU)  
2. Upload to GPU (VBO/EBO)  
3. Vertex shader:
   - displaces vertices  
   - computes normals  
4. Fragment shader:
   - computes lighting  
5. Render each frame  

---

## Build & Run

### Requirements

- OpenGL 3.3+
- C++17 compiler
- GLFW
- GLAD
- GLM

---

### Build (example with CMake)

```bash
mkdir build
cd build
cmake ..
make -j4
./build/bin/graphics
