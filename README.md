# CloudRender: Server-Side 3D Engine

CloudEngine is a **3D rendering engine using Vulkan**, designed to run on a server.  
Its main goal is **to minimise redundant server-side work during rendering** and stream every client viewport, allowing multiple clients to view and interact with a common scene efficiently even with minimal material.

## Motivations

CloudRender is an experimental server-side Vulkan renderer aimed at exploring multi-client streaming, shared world states, and minimal per-client overhead.

## Current Milestones

- Basic CPU → GPU mesh upload pipeline under construction.  


## Project Status / Roadmap

### Renderer Core
- [x] Vulkan instance and device creation  
- [x] Swapchain, render pass, and framebuffers  
- [x] Pipeline system  
- [x] Command buffers and command manager  
- [x] Synchronization system (fences, semaphores)  
- [x] Mesh abstraction (CPU mesh, GPU mesh buffers)  
- [ ] Swapchain recreation 
- [ ] Scene graph + per-object transforms  
- [ ] Render graph  

### Engine Layer :

- **Core Systems**
  - [ ] Engine initialization/shutdown sequence
  - [ ] Memory allocators (frame allocator, freelist, temp scratch)
  - [ ] Asset registry (tracking loaded meshes, textures, shaders)

- **Scene Representation**
  - [ ] Entity/Component system (lightweight, server-friendly)
  - [ ] Scene graph (parenting, local/global transforms)
  - [ ] Dirty flag propagation for transforms

- **Meshes & Geometry**
  - [ ] CPU-side mesh format (procedural + loaded)
  - [ ] GPU mesh manager (buffer pools, residency, defragmentation later)
  - [ ] Staging system for uploads (async ready)
  - [ ] Mesh instance system (per-client transforms)

- **Materials & Shaders**
  - [ ] Material descriptors (shader bindings, textures, uniforms)
  - [ ] Pipeline cache (reuse Vulkan pipelines from VulkanRenderer)
  - [ ] Shader compilation pipeline (GLSL/HLSL → SPIR-V with caching)

- **Rendering Abstractions**
  - [ ] High-level render commands → VulkanRenderer command generation
  - [ ] Renderable component (mesh + material)
  - [ ] Camera component (server defines view, clients override)
  - [ ] Render graph (simple pass system on top of VulkanRenderer)

- **Lighting**
  - [ ] Basic directional/point lights
  - [ ] Light buffer update system
  - [ ] Optional shadow mapping (single shadow map)

- **Culling & Visibility**
  - [ ] CPU frustum culling
  - [ ] (later) GPU culling / compute-based visibility

- **Procedural Tools**
  - [ ] Procedural mesh factory (cube, sphere, plane, terrain patch)
  - [ ] Procedural material generator (flat color, gradients, noise)

- **Server Integration**
  - [ ] Headless rendering interface (no swapchain)
  - [ ] Multi-viewport output (each client gets a camera)
  - [ ] Per-client scene queries (picking, bounds)
  - [ ] Network-friendly state diffing (only send changed transforms/light states)

- **Debugging / Tools**
  - [ ] In-engine logging & event markers
  - [ ] Frame stats interface (GPU time, draw count)
  - [ ] Visual debug primitives (lines, boxes, frustums)

### Tools & Utilities
- [x] RendererTest application  
- [ ] Debug UI  
- [ ] Hot-reload shaders  
- [ ] Serialization for meshes/materials  

---

## Project structure

```
CloudEngine/
├─ apps/
│ ├─ Client/ # Client application
│ │ ├─ CMakeLists.txt
│ │ └─ src/main.cpp
│ ├─ Server/ # Server application
│ │ ├─ CMakeLists.txt
│ │ └─ src/main.cpp
│ └─ RendererTest/ # Test utilities
│    ├─ CMakeLists.txt
│    └─ src/main.cpp
├─ libs/
│ └─  # Rendering library
│    ├─ CMakeLists.txt
│    ├─ include/VulkanRenderer.hpp
│    └─ src/VulkanRenderer.cpp
├─ CMakeLists.txt
├─ CMakePresets.json
├─ .gitignore
├─ vcpkg.json
└─ vcpkg-configuration.json
```

---

## Requirements

- **Vulkan SDK** installed locally.
- **vcpkg** for dependency management.
- **Visual Studio BuildTools** (or full compiler setup).
- **CMake** (≥ 3.21 recommended).
- **Ninja** (for building).

---

## Building

**Install dependencies via vcpkg:**
```powershell
cmake --preset=vcpkg
```
**Build:**
```powershell
cmake --build build --target Client
cmake --build build --target Server
cmake --build build --target VulkanRenderer
cmake --build build --target RendererTest
```

Each target corresponds to its respective module:

- Client → client application
- Server → server application
- VulkanRenderer → rendering library
- RendererTest → testing utilities
