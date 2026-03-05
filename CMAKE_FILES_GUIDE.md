# CMake文件清单和说明

## 📁 项目CMake文件结构

```
cmake/
├── CompilerOptions.cmake      # 编译器设置和警告配置
├── Dependencies.cmake         # 第三方依赖管理
├── Sources.cmake             # 源文件和包含目录配置
├── LinkLibraries.cmake       # 链接库配置
├── PostBuildSteps.cmake      # 构建后步骤（资源复制等）
└── Tests.cmake              # 单元测试配置（新增）

根目录
└── CMakeLists.txt           # 主构建文件
```

## 📋 各文件详细说明

### CMakeLists.txt（主构建文件）
**位置**: `/CMakeLists.txt`

**用途**: 项目的主构建配置文件

**内容组织**:
1. 项目定义和版本信息
2. 目标程序名称设置
3. 依次包含配置模块：
   - CompilerOptions.cmake
   - Dependencies.cmake
   - Sources.cmake
   - LinkLibraries.cmake
   - PostBuildSteps.cmake
   - Tests.cmake ✨ (新增)

**关键变量**:
- `TARGET`: 目标可执行程序名 (SunnyLand)
- `CMAKE_CURRENT_SOURCE_DIR`: 项目根目录

---

### cmake/CompilerOptions.cmake
**用途**: 配置C++编译标准和编译选项

**配置内容**:
```cmake
- C++20标准
- MSVC编译器: /W4警告级别
- GCC/Clang: -Wall -Wextra -Wpedantic
- 编译命令导出（供IDE使用）
```

**导出变量**: 无（直接设置全局选项）

---

### cmake/Dependencies.cmake ✨ (已更新)
**用途**: 管理所有第三方库的加载

**管理的依赖**:
```
- spdlog (日志库)
- SDL3 及其扩展
  - SDL3_image (图像加载)
  - SDL3_mixer (音频混合)
  - SDL3_ttf (文本渲染)
- GLM (数学库)
- nlohmann_json (JSON处理)
- ✨ Google Test (测试框架) - 新增
```

**Google Test配置**:
```cmake
# 使用FetchContent自动下载v1.14.0版本
# 支持BYPASS_GOOGLETEST选项跳过
```

**导出变量**: 无（直接添加子目录）

---

### cmake/Sources.cmake
**用途**: 定义源文件列表和包含目录

**定义的变量**:
- `TARGET_SOURCES`: 所有源文件列表
  ```
  引擎模块:
  - engine/core/ (config, game_app, time)
  - engine/input/ (input_manager)
  - engine/render/ (camera, renderer, sprite)
  - engine/resource/ (各种管理器)
  - engine/utils/ (工具类)
  
  主应用:
  - src/main.cpp
  - src/log.cpp
  ```

- `TARGET_INCLUDE_DIRS`: 包含目录
  ```
  - src/
  ```

**使用位置**: 在CMakeLists.txt中通过以下方式使用:
```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sources.cmake)
include_directories(${TARGET_INCLUDE_DIRS})
add_executable(${TARGET} ${TARGET_SOURCES})
```

---

### cmake/LinkLibraries.cmake
**用途**: 定义要链接的所有库

**定义的变量**:
- `TARGET_LINK_LIBS`: 链接库列表
  ```
  - spdlog::spdlog
  - SDL3::SDL3
  - SDL3_image::SDL3_image
  - SDL3_mixer::SDL3_mixer
  - SDL3_ttf::SDL3_ttf
  - glm::glm
  - nlohmann_json::nlohmann_json
  ```

**使用位置**:
```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LinkLibraries.cmake)
target_link_libraries(${TARGET} PRIVATE ${TARGET_LINK_LIBS})
```

---

### cmake/PostBuildSteps.cmake
**用途**: 定义编译后的步骤

**执行的操作**:
```cmake
# 将assets文件夹复制到可执行程序目录
add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${TARGET}>/assets"
    COMMENT "Copying assets folder to build directory"
)
```

**图解**:
```
编译完成
    ↓
检测POST_BUILD触发器
    ↓
执行copy_directory命令
    ↓
assets/ → build/assets/
```

---

### cmake/Tests.cmake ✨ (新增)
**用途**: 配置单元测试框架和测试模块

**主要功能**:

1. **测试选项配置**
   ```cmake
   option(ENABLE_TESTS "Enable unit tests" ON)
   ```
   - 可通过 `-DENABLE_TESTS=OFF` 禁用所有测试

2. **特定测试选择**
   ```cmake
   set(ONLY_TEST "" CACHE STRING "Only build specified test")
   ```
   - 可通过 `-DONLY_TEST=ConfigTest` 仅构建特定测试

3. **测试模块定义**
   ```cmake
   set(TEST_MODULES
       ConfigTest
       MathTest
       TimeTest
   )
   ```

4. **辅助函数**
   ```cmake
   function(add_engine_test test_name test_sources)
       # 创建可执行程序
       # 链接gtest库
       # 注册CTest
       # 打印状态信息
   endfunction()
   ```

5. **测试注册**
   - ConfigTest: `test/core/config_test.cpp`
   - MathTest: `test/utils/math_test.cpp`
   - TimeTest: `test/core/time_test.cpp`

6. **便利目标**
   ```cmake
   add_custom_target(run_tests
       COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
   )
   ```
   - 执行: `make run_tests`

**导出变量**: 无（仅在enable_testing()为ON时生效）

---

## 🔄 配置加载顺序

```
CMakeLists.txt
│
├─ CompilerOptions.cmake (第1步)
│  └─ 设置编译标准和选项
│
├─ Dependencies.cmake (第2步)
│  └─ 加载所有第三方库
│
├─ Sources.cmake (第3步)
│  └─ 定义源文件列表
│  └─ 定义包含目录
│
├─ LinkLibraries.cmake (第4步)
│  └─ 定义链接库
│
├─ 创建可执行文件 (第5步)
│  └─ add_executable(${TARGET} ${TARGET_SOURCES})
│
├─ 链接库到可执行文件 (第6步)
│  └─ target_link_libraries()
│
├─ PostBuildSteps.cmake (第7步)
│  └─ 配置编译后操作
│
└─ Tests.cmake (第8步)
   └─ 配置单元测试 (if ENABLE_TESTS)
```

## 📊 模块依赖关系

```
CMakeLists.txt (主导)
    │
    ├─→ CompilerOptions.cmake (独立)
    │
    ├─→ Dependencies.cmake (独立)
    │   └─ 为其他模块提供库
    │
    ├─→ Sources.cmake (独立)
    │   └─ 定义 TARGET_SOURCES
    │       TARGET_INCLUDE_DIRS
    │
    ├─→ LinkLibraries.cmake (独立)
    │   └─ 定义 TARGET_LINK_LIBS
    │       使用 Dependencies.cmake 中的库
    │
    ├─ [创建可执行程序]
    │
    ├─→ PostBuildSteps.cmake (依赖TARGET)
    │   └─ 使用 TARGET 变量
    │
    └─→ Tests.cmake (依赖SOURCE和LIBS)
        └─ 使用 TARGET_SOURCES
        └─ 使用 TARGET_INCLUDE_DIRS
        └─ 使用 TARGET_LINK_LIBS
```

## 🎯 使用示例

### 示例1: 构建主程序
```bash
cmake -B build -DENABLE_TESTS=OFF
cd build
make
```

### 示例2: 构建所有测试
```bash
cmake -B build -DENABLE_TESTS=ON
cd build
make
ctest --output-on-failure
```

### 示例3: 仅构建ConfigTest
```bash
cmake -B build -DENABLE_TESTS=ON -DONLY_TEST=ConfigTest
cd build
make
./ConfigTest
```

### 示例4: 添加新的编译器选项
编辑 `cmake/CompilerOptions.cmake`:
```cmake
if(MSVC)
    add_compile_options(/W4 /WX) # /WX 将警告视为错误
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
```

### 示例5: 添加新的第三方库
编辑 `cmake/Dependencies.cmake`:
```cmake
# 新库
add_subdirectory(third_party/newlib)
```

编辑 `cmake/LinkLibraries.cmake`:
```cmake
set(TARGET_LINK_LIBS
    # ... 现有库 ...
    newlib::newlib
)
```

## 📝 修改清单

### 最近修改

#### CompilerOptions.cmake
- ✅ 独立管理编译器配置

#### Dependencies.cmake (✨ 新增)
- ✅ 添加Google Test FetchContent配置
- ✅ 支持BYPASS_GOOGLETEST选项

#### Sources.cmake
- ✅ 集中管理所有源文件
- ✅ 定义包含目录变量

#### LinkLibraries.cmake
- ✅ 集中管理所有链接库
- ✅ 易于添加/删除库

#### PostBuildSteps.cmake
- ✅ 配置资源复制操作
- ✅ 确保assets在正确位置

#### Tests.cmake (✨ 新增)
- ✅ 完整的测试框架配置
- ✅ 支持灵活的测试选择
- ✅ 包含3个测试模块
- ✅ 提供便利的运行目标

#### CMakeLists.txt
- ✅ 模块化结构
- ✅ 清晰的组织顺序
- ✅ 易于维护和扩展

## 🔧 维护指南

### 添加新的源文件
1. 编辑 `cmake/Sources.cmake`
2. 在 `TARGET_SOURCES` 中添加文件路径

### 添加新的依赖库
1. 编辑 `cmake/Dependencies.cmake`
2. 添加 `add_subdirectory()` 或其他配置
3. 编辑 `cmake/LinkLibraries.cmake`
4. 在 `TARGET_LINK_LIBS` 中添加库名

### 添加新的测试模块
1. 创建测试文件: `test/module/module_test.cpp`
2. 编辑 `cmake/Tests.cmake`
3. 在 `TEST_MODULES` 中添加模块
4. 添加相应的 `add_engine_test()` 调用

### 修改编译选项
1. 编辑 `cmake/CompilerOptions.cmake`
2. 根据编译器类型修改选项

---

**最后更新**: 2026-03-05  
**总文件数**: 8 (CMakeLists.txt + 7个cmake模块)  
**状态**: ✅ 完整且就绪
