// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitem.h>
#include <scratchcpp/sprite.h>

class QBuffer;
class QNanoPainter;
class QOpenGLContext;

namespace scratchcpprender
{

class StageModel;
class SpriteModel;
class SceneMouseArea;

class IRenderedTarget : public QNanoQuickItem
{
    public:
        IRenderedTarget(QNanoQuickItem *parent = nullptr) :
            QNanoQuickItem(parent)
        {
        }

        virtual ~IRenderedTarget() { }

        virtual void updateVisibility(bool visible) = 0;
        virtual void updateX(double x) = 0;
        virtual void updateY(double y) = 0;
        virtual void updateSize(double size) = 0;
        virtual void updateDirection(double direction) = 0;
        virtual void updateRotationStyle(libscratchcpp::Sprite::RotationStyle style) = 0;
        virtual void updateLayerOrder(int layerOrder) = 0;

        virtual void loadCostume(libscratchcpp::Costume *costume) = 0;

        virtual void beforeRedraw() = 0;

        virtual libscratchcpp::IEngine *engine() const = 0;
        virtual void setEngine(libscratchcpp::IEngine *newEngine) = 0;

        virtual StageModel *stageModel() const = 0;
        virtual void setStageModel(StageModel *newStageModel) = 0;

        virtual SpriteModel *spriteModel() const = 0;
        virtual void setSpriteModel(SpriteModel *newSpriteModel) = 0;

        virtual libscratchcpp::Target *scratchTarget() const = 0;

        virtual SceneMouseArea *mouseArea() const = 0;
        virtual void setMouseArea(SceneMouseArea *newMouseArea) = 0;

        virtual double stageScale() const = 0;
        virtual void setStageScale(double scale) = 0;

        virtual qreal width() const = 0;
        virtual void setWidth(qreal width) = 0;

        virtual qreal height() const = 0;
        virtual void setHeight(qreal width) = 0;

        virtual QPointF mapFromScene(const QPointF &point) const = 0;

        virtual QBuffer *bitmapBuffer() = 0;
        virtual const QString &bitmapUniqueKey() const = 0;

        virtual void lockCostume() = 0;
        virtual void unlockCostume() = 0;

        virtual bool mirrorHorizontally() const = 0;

        virtual bool isSvg() const = 0;
        virtual void paintSvg(QNanoPainter *painter) = 0;

        virtual void updateHullPoints(QOpenGLFramebufferObject *fbo) = 0;
        virtual const std::vector<QPointF> &hullPoints() const = 0;
};

} // namespace scratchcpprender
