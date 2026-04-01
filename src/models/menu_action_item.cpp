// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "menu_action_item.h"

MenuActionItem::MenuActionItem()
    : positionNumber(1)
    , positionNumberSingleFile(1)
    , positionNumberMultiFiles(1)
    , separatorTop(false)
    , separatorBottom(false)
    , isRoot(false)
    , level(1)
    , isSystem(false)
{
}
