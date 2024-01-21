// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <ipenlayer.h>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <scratchcpp/iengine.h>

namespace scratchcpprender
{

class PenLayer : public IPenLayer
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)

    public:
        PenLayer(QNanoQuickItem *parent = nullptr);
        ~PenLayer();

        libscratchcpp::IEngine *engine() const override;
        void setEngine(libscratchcpp::IEngine *newEngine) override;

        void clear() override;
        void drawPoint(const PenAttributes &penAttributes, double x, double y) override;
        void drawLine(const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) override;

        QOpenGLFramebufferObject *framebufferObject() const override;

        static IPenLayer *getProjectPenLayer(libscratchcpp::IEngine *engine);

    signals:
        void engineChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;

    private:
        static std::unordered_map<libscratchcpp::IEngine *, IPenLayer *> m_projectPenLayers;
        libscratchcpp::IEngine *m_engine = nullptr;
        std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
        std::unique_ptr<QOpenGLPaintDevice> m_paintDevice;
        std::unique_ptr<QPainter> m_painter;
        QOpenGLFramebufferObjectFormat m_fboFormat;
};

} // namespace scratchcpprender
