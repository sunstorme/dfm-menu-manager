// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QObject>

class Logger : public QObject {
    Q_OBJECT
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };
    Q_ENUM(Level)

    static void log(Level level, const QString &message,
                   const QString &file = "", int line = 0);

    static void setLogLevel(Level level);

    static Level getLogLevel();

private:
    static Level s_logLevel;
    static QString levelToString(Level level);
};

#define LOG_DEBUG(msg) Logger::log(Logger::Level::Debug, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::log(Logger::Level::Info, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::log(Logger::Level::Warning, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::log(Logger::Level::Error, msg, __FILE__, __LINE__)

#endif // LOGGER_H
