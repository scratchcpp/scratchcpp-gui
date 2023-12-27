// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "stagemodel.h"
#include "renderedtarget.h"

using namespace scratchcpprender;

StageModel::StageModel(QObject *parent) :
    QObject(parent)
{
}

void StageModel::init(libscratchcpp::Stage *stage)
{
    m_stage = stage;

    if (m_renderedTarget && stage)
        m_renderedTarget->loadCostume(stage->currentCostume().get());
}

void StageModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    if (m_renderedTarget)
        m_renderedTarget->loadCostume(costume);
}

void StageModel::onTempoChanged(int tempo)
{
}

void StageModel::onVideoStateChanged(libscratchcpp::Stage::VideoState videoState)
{
}

void StageModel::onVideoTransparencyChanged(int videoTransparency)
{
}

void StageModel::onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value)
{
}

void StageModel::onGraphicsEffectsCleared()
{
}

libscratchcpp::Stage *StageModel::stage() const
{
    return m_stage;
}

IRenderedTarget *StageModel::renderedTarget() const
{
    return m_renderedTarget;
}

void StageModel::setRenderedTarget(IRenderedTarget *newRenderedTarget)
{
    if (m_renderedTarget == newRenderedTarget)
        return;

    m_renderedTarget = newRenderedTarget;

    if (m_renderedTarget && m_stage)
        m_renderedTarget->loadCostume(m_stage->currentCostume().get());

    emit renderedTargetChanged();
}
