# DFM 右键菜单管理器 - 项目规格文档

> **版本**: 1.0.0
> **状态**: 开发中
> **更新**: 2024-04-03

---

## 📋 执行摘要

**项目名称**: dfm-menu-manager
**技术栈**: QML + C++ Qt6
**目标**: 提供可视化的DFM文件管理器右键菜单配置管理工具
**核心价值**: 简化复杂的配置文件编辑过程，提供直观的图形界面

### 核心功能（MVP）
- ✅ 配置文件管理（用户级/系统级）
- ✅ 可视化菜单树编辑（最多3级）
- ✅ 属性编辑面板
- ✅ 权限智能处理
- ✅ 自动备份机制
- ✅ 中英文双语支持

---

## 🎯 产品定位

### 目标用户
- Deepin/Ubuntu Linux用户
- 需要自定义文件管理器右键菜单的高级用户
- 系统管理员

### 解决痛点
- **问题**: 手动编辑INI配置文件容易出错
- **问题**: 不了解配置文件格式的用户无法自定义菜单
- **问题**: 系统配置权限处理复杂
- **方案**: 提供图形化界面，自动处理权限和格式验证

---

## 🏗️ 系统架构

### 架构分层

```
┌─────────────────────────────────────────┐
│         UI层 (QML)                      │
│  三列布局：文件管理 | 菜单编辑 | 属性面板 │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│      业务逻辑层 (C++)                    │
│  MenuManager | ConfigParser | Writer    │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│      数据模型层 (C++)                    │
│  MenuTreeModel | MenuFileModel | Item   │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│      存储层                              │
│  ~/.local/share/... | /usr/share/...    │
└─────────────────────────────────────────┘
```

### 核心模块

| 模块 | 职责 | 关键类 |
|------|------|--------|
| **配置管理** | 解析/写入/验证配置文件 | `ConfigParser`, `ConfigWriter` |
| **菜单管理** | 协调各模块，状态管理 | `MenuManager` |
| **数据模型** | 树形结构、文件列表 | `MenuTreeModel`, `MenuFileModel` |
| **UI组件** | 三列布局、属性编辑 | QML组件 |
| **文件监控** | 实时监控配置变化 | `FileWatcher` |

---

## 📊 数据模型设计

### MenuActionItem（核心数据结构）

```cpp
struct MenuActionItem {
    // 标识
    QString id;              // 唯一ID
    bool isRoot;             // 是否根节点
    int level;               // 层级 (0-3)

    // 显示
    QString name;            // 菜单名称
    QString nameLocal;       // 本地化名称
    QString comment;         // 描述

    // 配置
    QStringList menuTypes;   // SingleFile, MultiFiles等
    QStringList supportSuffix; // 文件后缀过滤
    int positionNumber;      // 显示位置
    bool separatorTop;       // 顶部分隔符
    bool separatorBottom;    // 底部分隔符

    // 动作
    QString execCommand;     // 执行命令
    QStringList childActions; // 子菜单ID列表

    // 元数据
    QString configFile;      // 所属配置文件
    bool isSystem;           // 是否系统配置
};
```

### MenuTreeModel（树形模型）

- **继承**: `QAbstractItemModel`
- **角色**: NameRole, IdRole, LevelRole, IsEditableRole等
- **操作**: addItem, removeItem, moveItem, updateItem

---

## 🎨 UI设计

### 主界面布局

```
┌──────────────────────────────────────────────────────┐
│  文件管理面板 │    菜单编辑面板      │  属性编辑面板  │
│  (25% 宽度)  │      (45% 宽度)     │   (30% 宽度)   │
│              │                     │                │
│  🔍 搜索     │  🌳 菜单树结构      │  📝 名称       │
│              │                     │                │
│  📁 用户配置 │  ├─ 子菜单1         │  ⚡ 命令       │
│    ├ 文件1   │  │  └─ 子项1.1      │  🏷️ 类型       │
│    └ 文件2   │  └─ 子菜单2         │  📎 后缀       │
│              │                     │  🔢 位置       │
│  🖥️ 系统配置 │  [支持拖拽重排]     │  ✂️ 分隔符     │
│    ├ 文件3   │                     │                │
│    └ 文件4   │                     │  💾 保存       │
└──────────────────────────────────────────────────────┘
```

### 关键交互

| 操作 | 快捷键 | 右键菜单 |
|------|--------|----------|
| 新建子菜单 | `Ctrl+N` | ✅ |
| 删除 | `Delete` | ✅ |
| 保存 | `Ctrl+S` | - |
| 上移/下移 | `Ctrl+↑/↓` | ✅ |
| 复制/粘贴 | `Ctrl+C/V` | ✅ |

---

## 🔧 关键技术点

### 1. 配置文件解析
- **格式**: INI格式（Desktop Entry Specification）
- **解析器**: 手写解析器（兼容性更好）
- **验证**: 完整性检查、层级限制、必填字段

### 2. 树形模型
- **实现**: `QAbstractItemModel`
- **优化**: 懒加载子节点
- **限制**: 最多3级嵌套

### 3. 权限处理
- **用户配置**: 直接读写
- **系统配置**: 使用`pkexec`提升权限
- **安全**: 修改前自动备份（.bak文件）

### 4. 国际化
- **工具**: Qt Linguist
- **语言**: 中文（zh_CN）、英文（en_US）
- **加载**: 根据系统语言自动切换

---

## 📁 配置文件格式

### 目录结构
```
用户级: ~/.local/share/deepin/dde-file-manager/context-menus/
系统级: /usr/share/applications/context-menus/
```

### 文件示例
```ini
[Menu Entry]
Comment=媒体分析工具
Comment[zh_CN]=强大的媒体分析软件
Version=1.0
Actions=MediaInfo:Basic

[Menu Action MediaInfo]
Name=MediaInfo
Name[zh_CN]=媒体信息
X-DFM-MenuTypes=SingleFile
X-DFM-SupportSuffix=mp4:avi:mkv:jpg:png
PosNum=1
Actions=MetaInfo:FrameInfo

[Menu Action MetaInfo]
Name=MetaInfo
Name[zh_CN]=元信息
Exec=/usr/bin/media-analyzer --meta %F
```

---

## 🗺️ 开发路线图

### Phase 1: 基础框架 ✅（已完成）
- [x] 项目结构搭建
- [x] CMake配置
- [x] 基础数据模型
- [x] 配置解析器

### Phase 2: 核心功能 🚧（进行中）
- [ ] 配置写入器
- [ ] 菜单树模型完善
- [ ] 基础UI框架
- [ ] 文件监控

### Phase 3: UI完善 📋（待开始）
- [ ] 三列布局实现
- [ ] 属性编辑面板
- [ ] 拖拽功能
- [ ] 快捷键系统

### Phase 4: 高级功能 🔮（规划中）
- [ ] 权限提升
- [ ] 窗口状态保存
- [ ] 国际化完善
- [ ] 错误处理优化

### Phase 5: 测试与发布 🎯（待开始）
- [ ] 单元测试
- [ ] 集成测试
- [ ] 性能优化
- [ ] 打包发布

---

## ⚠️ 风险管理

### 技术风险
| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| Qt6兼容性 | 高 | 使用官方文档，充分测试 |
| 权限提升复杂性 | 中 | 封装polkit接口，提供fallback |
| 配置文件格式变化 | 中 | 版本检测，向后兼容 |

### 进度风险
| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 功能蔓延 | 高 | 严格按MVP范围，延后非核心功能 |
| 资源不足 | 中 | 分阶段交付，优先核心功能 |

---

## 📦 交付物

### 代码交付
- [ ] 源代码（C++ + QML）
- [ ] 单元测试覆盖率 > 70%
- [ ] API文档（Doxygen）

### 用户交付
- [ ] 可执行文件
- [ ] Debian包（.deb）
- [ ] 用户手册

### 开发者交付
- [ ] 技术文档（本文档）
- [ ] 构建脚本
- [ ] 示例配置文件

---

## 🚀 快速开始

### 环境要求
```bash
# 安装依赖
sudo apt install build-essential cmake \
    qt6-base-dev qt6-declarative-dev qt6-tools-dev
```

### 构建项目
```bash
git clone <repo-url>
cd dfm-menu-manager
mkdir build && cd build
cmake ..
make -j$(nproc)
./dfm-menu-manager
```

---

## 📚 参考资料

- [Qt6官方文档](https://doc.qt.io/qt-6/)
- [Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/)
- [DFM源码](https://github.com/linuxdeepin/dde-file-manager)

---

## 📝 变更记录

| 版本 | 日期 | 变更内容 | 作者 |
|------|------|----------|------|
| 1.0.0 | 2024-04-03 | 初始版本，整合架构设计 | zhanghongyuan |

---

**注意**: 本文档是项目的权威规格说明，所有技术决策应以本文档为准。
