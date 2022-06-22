#pragma once

#include <QDir>
#include <QString>

struct VideoWriterOptions
{
    QDir outputDir;
    QString outputExtension;
    int fourcc{};
    double fps = 20;
    int width = 100;
    int height = 100;
};
