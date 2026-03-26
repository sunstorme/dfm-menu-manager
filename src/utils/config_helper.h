#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QString>

class ConfigHelper {
public:
    static bool isValidConfigFile(const QString &filePath);
};

#endif // CONFIGHELPER_H
