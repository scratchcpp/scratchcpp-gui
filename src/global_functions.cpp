// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QQuickWindow>
#include <scratchcpp-render/scratchcpp-render.h>

void scratchcpprender::init()
{
    qputenv("QSG_RENDER_LOOP", "basic");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
}

const std::string &scratchcpprender::version()
{
    static const std::string ret = SCRATCHCPPRENDER_VERSION;
    return ret;
}

int scratchcpprender::majorVersion()
{
    return SCRATCHCPPRENDER_VERSION_MAJOR;
}

int scratchcpprender::minorVersion()
{
    return SCRATCHCPPRENDER_VERSION_MINOR;
}

int scratchcpprender::patchVersion()
{
    return SCRATCHCPPRENDER_VERSION_PATCH;
}
