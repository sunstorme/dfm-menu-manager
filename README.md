# DFM 右键菜单管理器

一个用于管理 Deepin 文件管理器 (DFM) 右键菜单配置的可视化工具。

## 项目简介

DFM 右键菜单管理器是一个基于 QML + C++ Qt6 开发的桌面应用程序,旨在提供一个直观、易用的界面来管理和配置 DFM 文件管理器的右键菜单。

### 主要功能

- 📁 **配置文件管理**: 支持用户级和系统级配置文件的新建、编辑、删除
- 🌳 **可视化编辑**: 树形结构展示菜单层次,支持拖拽和可视化编辑
- 🎨 **属性编辑**: 完整的菜单属性编辑面板,包括名称、命令、类型、后缀等
- 🔒 **权限管理**: 智能处理系统配置文件的权限问题
- 💾 **自动备份**: 修改配置前自动备份原文件
- 🖥️ **窗口状态**: 记忆窗口位置、大小和列宽
- ⌨️ **快捷键支持**: 完整的快捷键系统,提高操作效率
- 🌐 **国际化**: 支持中英文双语界面

## 技术栈

- **语言**: C++17, QML
- **框架**: Qt 6.2+
- **构建工具**: CMake 3.16+
- **可选依赖**: DTK (Deepin Tool Kit)

## 项目结构

```
dfm-menu-manager/
├── CMakeLists.txt              # CMake 构建配置
├── README.md                   # 项目说明
├── LICENSE                     # GPL-3.0+ 许可证
├── demo.conf                   # 示例配置文件
├── .gitignore                  # Git 忽略规则
├── src/                        # C++ 源代码
│   ├── main.cpp                # 程序入口
│   ├── core/                   # 核心业务逻辑
│   │   ├── menu_manager.h/cpp      # 菜单管理器
│   │   ├── config_parser.h/cpp     # 配置文件解析器
│   │   ├── config_writer.h/cpp     # 配置文件写入器
│   │   └── file_watcher.h/cpp      # 文件变化监控
│   ├── models/                 # 数据模型
│   │   ├── menu_tree_model.h/cpp   # 菜单树形模型
│   │   ├── menu_file_model.h/cpp   # 文件列表模型
│   │   └── menu_action_item.h/cpp  # 菜单项数据结构
│   ├── utils/                  # 工具类
│   │   ├── config_helper.h/cpp     # 配置辅助工具
│   │   ├── file_utils.h/cpp        # 文件操作工具
│   │   └── window_manager.h/cpp    # 窗口状态管理
│   └── resources/              # 资源文件
│       ├── icons/                 # 图标资源
│       ├── qml/                   # QML 资源
│       └── qml.qrc                # QML 资源文件
├── qml/                        # QML 界面文件
│   ├── main.qml                # 主窗口（包含所有 UI 组件）
│   ├── components/             # 可复用组件目录
│   └── styles/                 # 样式文件
│       ├── qmldir                 # QML 模块定义
│       └── Style.qml              # 全局样式
├── translations/               # 翻译文件
│   ├── dfm-menu-manager_en_US.ts # 英文翻译
│   └── dfm-menu-manager_zh_CN.ts # 中文翻译
├── debian/                     # Debian 打包文件
│   ├── control                     # 包元数据
│   ├── changelog                  # 变更日志
│   ├── rules                       # 构建规则
│   ├── copyright                   # 版权信息
│   ├── dfm-menu-manager.desktop    # 桌面入口
│   └── dfm-menu-manager.svg        # 应用图标
├── plans/                      # 项目文档
│   ├── architecture.md         # 架构文档
│   ├── detailed-design.md      # 详细设计文档
│   └── implementation-guide.md # 实施指南
└── tests/                      # 单元测试目录
    └── CMakeLists.txt          # 测试配置
```

## 配置文件格式

DFM 右键菜单使用 INI 格式的配置文件,支持最多 3 级菜单嵌套。

### 配置文件示例

```ini
[Menu Entry]
Comment=A powerful media file analysis tool
Comment[zh_CN]=一个强大的媒体分析软件
Version=1.0
Actions=MediaInfo:Basic

[Menu Action MediaInfo]
Name=MediaInfo
Name[zh_CN]=媒体信息
X-DFM-MenuTypes=SingleFile
X-DFM-SupportSuffix=mp4:avi:mkv:jpg:png
PosNum=1
Actions=MetaInfo:FrameInfo:PacketInfo

[Menu Action MetaInfo]
Name=MetaInfo
Name[zh_CN]=元信息
PosNum=1
Actions=Streams:Format

[Menu Action Streams]
Name=Streams
Name[zh_CN]=流信息
PosNum=1
Exec=/usr/bin/media-analyzer --streams -m %F
```

### 配置目录

- **用户级**: `~/.local/share/deepin/dde-file-manager/context-menus`
- **系统级**: `/usr/share/applications/context-menus/`

## 快速开始

### 环境要求

- Deepin V20/V25 或其他 Linux 发行版
- GCC 9.0+ 或 Clang 10.0+
- CMake 3.16+
- Qt 6.2+

### 安装依赖

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-declarative-dev qt6-tools-dev
```

### 构建项目

```bash
# 克隆项目
git clone https://github.com/add-uos/dfm-menu-manager.git
cd dfm-menu-manager

# 创建构建目录
mkdir build && cd build

# 配置和编译
cmake ..
make -j$(nproc)

# 运行
./dfm-menu-manager
```

### 构建 Debian 包

```bash
# 使用 deb-builder-launcher.sh 构建
deb-builder-launcher.sh

# 或使用标准 Debian 打包命令
dpkg-buildpackage -us -uc -b

# 安装生成的包
sudo dpkg -i ../dfm-menu-manager_*.deb
```

## 使用说明

### 界面布局

应用程序采用三列布局:

1. **左侧面板**: 文件管理区
   - 搜索框
   - 用户配置文件列表(可折叠)
   - 系统配置文件列表(可折叠)

2. **中间面板**: 菜单编辑区
   - 树形菜单结构
   - 支持拖拽重排序
   - 右键菜单操作

3. **右侧面板**: 属性编辑区
   - 菜单名称
   - 可执行命令
   - 菜单显示类型
   - 文件后缀过滤
   - 位置编号
   - 分隔符选项

### 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+N` | 新建子菜单项 |
| `Ctrl+Shift+N` | 新建配置文件 |
| `Delete` | 删除选中项 |
| `Ctrl+S` | 保存当前配置 |
| `Ctrl+O` | 打开配置文件 |
| `Ctrl+F` | 聚焦搜索框 |
| `Ctrl+Up` | 上移菜单项 |
| `Ctrl+Down` | 下移菜单项 |
| `Ctrl+C` | 复制菜单项 |
| `Ctrl+V` | 粘贴菜单项 |
| `F2` | 重命名 |
| `Escape` | 取消当前操作 |
| `Ctrl+Q` | 退出应用 |

### 右键菜单

**文件列表右键菜单**:
- 新建配置文件
- 打开
- 重命名
- 复制到系统/用户目录
- 删除
- 刷新

**菜单树右键菜单**:
- 添加子菜单
- 添加兄弟菜单
- 删除
- 复制
- 粘贴
- 上移
- 下移
- 展开/折叠全部

## 开发指南

详细的开发文档请参考:

- [架构文档](docs/architecture.md) - 整体架构设计
- [详细设计文档](docs/detailed-design.md) - 数据模型和类设计
- [实施指南](docs/implementation-guide.md) - 开发实施步骤

### 核心类说明

#### MenuManager (菜单管理器)

负责配置文件的加载、保存和管理,是整个应用的核心控制器。

```cpp
class MenuManager : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void loadConfigurations();
    Q_INVOKABLE bool saveConfiguration(const QString &filePath);
    Q_INVOKABLE bool createNewConfig(const QString &name, bool isSystem);
    Q_INVOKABLE bool deleteConfig(const QString &filePath);
    Q_INVOKABLE MenuTreeModel* getMenuModel(const QString &configFile);
    // ...
};
```

#### ConfigParser (配置解析器)

解析 DFM 配置文件格式,将文本转换为数据结构。

```cpp
class ConfigParser {
public:
    struct ConfigData {
        QString version;
        QString comment;
        QList<MenuActionItem> actions;
        // ...
    };
    
    static ConfigData parseFile(const QString &filePath);
    static bool validate(const ConfigData &data);
    // ...
};
```

#### MenuTreeModel (菜单树模型)

为 QML 提供树形数据模型,支持菜单项的增删改查。

```cpp
class MenuTreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
    Q_INVOKABLE void addItem(const QModelIndex &parent, const QString &name);
    Q_INVOKABLE void removeItem(const QModelIndex &index);
    Q_INVOKABLE void moveItem(const QModelIndex &index, int direction);
    // ...
};
```

## 测试

### 运行单元测试

```bash
cd build
ctest --output-on-failure
```

### 运行特定测试

```bash
./tests/test_config_parser
./tests/test_menu_manager
```

## 贡献指南

欢迎贡献代码、报告问题或提出建议!

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 代码规范

- 遵循 Qt 编码规范
- 使用 C++17 特性
- QML 文件使用 4 空格缩进
- 添加必要的注释和文档

## 许可证

本项目采用 **GPL-3.0+** 许可证（GNU General Public License version 3 or later）- 详见 [LICENSE](LICENSE) 文件

## 致谢

- Deepin 团队提供的 DFM 文件管理器
- Qt 社区的优秀文档和支持
- UnionTech 提供的开发支持

## 联系方式

- **维护者**: zhanghongyuan <zhanghongyuan@uniontech.com>
- **项目主页**: [https://github.com/zhanghongyuan/dfm-menu-manager](https://github.com/zhanghongyuan/dfm-menu-manager)
- **问题反馈**: [Issues](https://github.com/zhanghongyuan/dfm-menu-manager/issues)
- **邮箱**: zhanghongyuan@uniontech.com

## 更新日志

### v1.0.0 (待发布)

- ✨ 初始版本
- 🎨 完整的 UI 界面
- 📝 配置文件解析和写入
- 🌳 树形菜单编辑器
- 🔧 属性编辑面板
- ⌨️ 快捷键支持
- 🌐 国际化支持

---

**注意**: 本项目正在积极开发中,部分功能可能尚未完成。欢迎参与贡献!
