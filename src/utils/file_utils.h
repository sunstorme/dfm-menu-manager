#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils {
public:
    static QString getUserConfigDir();
    static QString getSystemConfigDir();
    static bool ensureDirExists(const QString &path);
};

#endif // FILEUTILS_H
