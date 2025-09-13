# CloudRender: Server-Side 3D Engine

CloudEngine is a **3D rendering engine using Vulkan**, designed to run in the cloud.  
Its main goal is **to minimise redundant server-side work during rendering**, allowing multiple clients to view the scene efficiently, even with minimal material.

---

## 🎨 Project structure

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
│ ├─ CMakeLists.txt
│ └─ src/main.cpp
├─ libs/
│ └─ VulkanRenderer/ # Rendering library
│ ├─ CMakeLists.txt
│ ├─ include/VulkanRenderer/VulkanRenderer.hpp
│ └─ src/VulkanRenderer.cpp
├─ CMakeLists.txt
├─ CMakePresets.json
├─ .gitignore
├─ vcpkg.json
└─ vcpkg-configuration.json
```

---

## ⚙️ Requirements

- **Vulkan SDK** installed locally.
- **vcpkg** for dependency management.
- **Visual Studio BuildTools** (or full compiler setup).
- **CMake** (≥ 3.21 recommended).
- **Ninja** (for building).

---

## 🚀 Building

1. **Install dependencies via vcpkg:**

```powershell
cmake --preset=vcpkg

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