﻿// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQmlEngine>
#include <QFuture>
#include <scratchcpp/project.h>
#include <scratchcpp/iengine.h>

#include "stagemodel.h"

Q_MOC_INCLUDE("spritemodel.h");

namespace scratchcpprender
{

class SpriteModel;

class ProjectLoader : public QObject
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
        Q_PROPERTY(bool loadStatus READ loadStatus NOTIFY loadStatusChanged)
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stage READ stage NOTIFY stageChanged)
        Q_PROPERTY(QQmlListProperty<SpriteModel> sprites READ sprites NOTIFY spritesChanged)
        Q_PROPERTY(QQmlListProperty<SpriteModel> clones READ clones NOTIFY clonesChanged)
        Q_PROPERTY(double fps READ fps WRITE setFps NOTIFY fpsChanged)
        Q_PROPERTY(bool turboMode READ turboMode WRITE setTurboMode NOTIFY turboModeChanged)
        Q_PROPERTY(unsigned int stageWidth READ stageWidth WRITE setStageWidth NOTIFY stageWidthChanged)
        Q_PROPERTY(unsigned int stageHeight READ stageHeight WRITE setStageHeight NOTIFY stageHeightChanged)
        Q_PROPERTY(int cloneLimit READ cloneLimit WRITE setCloneLimit NOTIFY cloneLimitChanged)
        Q_PROPERTY(bool spriteFencing READ spriteFencing WRITE setSpriteFencing NOTIFY spriteFencingChanged)
        Q_PROPERTY(unsigned int downloadedAssets READ downloadedAssets NOTIFY downloadedAssetsChanged)
        Q_PROPERTY(unsigned int assetCount READ assetCount NOTIFY assetCountChanged)

    public:
        explicit ProjectLoader(QObject *parent = nullptr);
        ~ProjectLoader();

        const QString &fileName() const;
        void setFileName(const QString &newFileName);

        bool loadStatus() const;

        libscratchcpp::IEngine *engine() const;
        void setEngine(libscratchcpp::IEngine *engine);

        StageModel *stage();

        QQmlListProperty<SpriteModel> sprites();
        const QList<SpriteModel *> &spriteList() const;

        QQmlListProperty<SpriteModel> clones();
        const QList<SpriteModel *> &cloneList() const;

        Q_INVOKABLE void start();
        Q_INVOKABLE void stop();

        double fps() const;
        void setFps(double newFps);

        bool turboMode() const;
        void setTurboMode(bool newTurboMode);

        unsigned int stageWidth() const;
        void setStageWidth(unsigned int newStageWidth);

        unsigned int stageHeight() const;
        void setStageHeight(unsigned int newStageHeight);

        int cloneLimit() const;
        void setCloneLimit(int newCloneLimit);

        bool spriteFencing() const;
        void setSpriteFencing(bool newSpriteFencing);

        unsigned int downloadedAssets() const;

        unsigned int assetCount() const;

    signals:
        void fileNameChanged();
        void loadStatusChanged();
        void loadingFinished();
        void engineChanged();
        void stageChanged();
        void spritesChanged();
        void clonesChanged();
        void fpsChanged();
        void turboModeChanged();
        void stageWidthChanged();
        void stageHeightChanged();
        void cloneLimitChanged();
        void spriteFencingChanged();
        void downloadedAssetsChanged();
        void assetCountChanged();
        void cloneCreated(SpriteModel *model);
        void cloneDeleted(SpriteModel *model);

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        static void callLoad(ProjectLoader *loader);
        void load();
        void initTimer();
        void redraw();
        void addClone(SpriteModel *model);
        void deleteClone(SpriteModel *model);

        int m_timerId = -1;
        QString m_fileName;
        QFuture<void> m_loadThread;
        libscratchcpp::Project m_project;
        libscratchcpp::IEngine *m_engine = nullptr;
        QMutex m_engineMutex;
        bool m_loadStatus = false;
        StageModel m_stage;
        QList<SpriteModel *> m_sprites;
        QList<SpriteModel *> m_clones;
        double m_fps = 30;
        bool m_turboMode = false;
        unsigned int m_stageWidth = 480;
        unsigned int m_stageHeight = 360;
        int m_cloneLimit = 300;
        bool m_spriteFencing = true;
        std::atomic<unsigned int> m_downloadedAssets = 0;
        std::atomic<unsigned int> m_assetCount = 0;
        std::atomic<bool> m_stopLoading = false;
};

} // namespace scratchcpprender
