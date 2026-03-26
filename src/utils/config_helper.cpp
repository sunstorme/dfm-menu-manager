#include "config_helper.h"
#include <QFileInfo>

bool ConfigHelper::isValidConfigFile(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.suffix() == "conf";
}
