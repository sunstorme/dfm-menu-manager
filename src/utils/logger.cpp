// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "logger.h"
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>

Logger::Level Logger::s_logLevel = Logger::Level::Info;

void Logger::log(Level level, const QString &message, const QString &file, int line) {
    if (level < s_logLevel) {
        return;
    }

    QString levelStr = levelToString(level);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString location;

    if (!file.isEmpty()) {
        QFileInfo fileInfo(file);
        location = QString(" [%1:%2]").arg(fileInfo.fileName()).arg(line);
    }

    QString fullMessage = QString("[%1] [%2]%3 %4")
        .arg(timestamp)
        .arg(levelStr)
        .arg(location)
        .arg(message);

    switch (level) {
    case Level::Debug:
        qDebug().noquote() << fullMessage;
        break;
    case Level::Info:
        qInfo().noquote() << fullMessage;
        break;
    case Level::Warning:
        qWarning().noquote() << fullMessage;
        break;
    case Level::Error:
        qCritical().noquote() << fullMessage;
        break;
    }
}

void Logger::setLogLevel(Level level) {
    s_logLevel = level;
}

Logger::Level Logger::getLogLevel() {
    return s_logLevel;
}

QString Logger::levelToString(Level level) {
    switch (level) {
    case Level::Debug:
        return "DEBUG";
    case Level::Info:
        return "INFO";
    case Level::Warning:
        return "WARNING";
    case Level::Error:
        return "ERROR";
    }
    return "UNKNOWN";
}
