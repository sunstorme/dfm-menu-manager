// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QString>

namespace Constants {
    // 时间相关常量
    constexpr int MAX_MENU_LEVEL = 3;
    constexpr int FILE_CHANGE_DELAY_MS = 100;
    constexpr int FILE_WATCHER_RESTART_DELAY_MS = 50;
    constexpr int AUTO_REFRESH_DELAY_MS = 200;

    // 字符串操作常量
    constexpr int BRACKET_OFFSET = 1;
    constexpr int MENU_ACTION_PREFIX_LENGTH = 12;

    // QML注册常量
    constexpr int QML_MAJOR_VERSION = 1;
    constexpr int QML_MINOR_VERSION = 0;
    constexpr const char* QML_MODULE_NAME = "DFMMenu";

    namespace Defaults {
        constexpr const char* DEFAULT_VERSION = "1.0";
        constexpr const char* DEFAULT_APP_VERSION = "1.0.0";
        constexpr const char* DEFAULT_COMMENT = "New configuration";
        constexpr const char* DEFAULT_COMMENT_LOCAL = "新配置";
        constexpr const char* ROOT_ACTION_ID = "root";
        constexpr int ROOT_LEVEL = 0;
    }

    namespace File {
        constexpr const char* CONFIG_EXTENSION = ".conf";
        constexpr const char* BACKUP_EXTENSION = ".bak";
        constexpr const char* CONFIG_FILE_FILTER = "*.conf";
    }

    namespace Config {
        // 组名
        constexpr const char* MENU_ENTRY_GROUP = "Menu Entry";
        constexpr const char* MENU_ACTION_PREFIX = "Menu Action ";

        // 键名
        constexpr const char* KEY_NAME = "Name";
        constexpr const char* KEY_COMMENT = "Comment";
        constexpr const char* KEY_VERSION = "Version";
        constexpr const char* KEY_ACTIONS = "Actions";
        constexpr const char* KEY_EXEC = "Exec";
        constexpr const char* KEY_POS_NUM = "PosNum";
        constexpr const char* KEY_SEPARATOR = "Separator";

        // 复合键名
        constexpr const char* KEY_NAME_LOCAL = "Name[zh_CN]";
        constexpr const char* KEY_COMMENT_LOCAL = "Comment[zh_CN]";
        constexpr const char* KEY_POS_NUM_SINGLE = "PosNum-SingleFile";
        constexpr const char* KEY_POS_NUM_MULTI = "PosNum-MultiFiles";
        constexpr const char* KEY_MENU_TYPES = "X-DFM-MenuTypes";
        constexpr const char* KEY_SUPPORT_SUFFIX = "X-DFM-SupportSuffix";

        // 分隔符值
        constexpr const char* SEPARATOR_TOP = "Top";
        constexpr const char* SEPARATOR_BOTTOM = "Bottom";
        constexpr const char* SEPARATOR_NONE = "";

        // X-DFM前缀
        constexpr const char* X_DFM_PREFIX = "X-DFM-";
    }
}
