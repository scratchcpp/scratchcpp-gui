#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <stagemodel.h>
#include <graphicseffect.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(StageModelTest, Constructors)
{
    StageModel model1;
    StageModel model2(&model1);
    ASSERT_EQ(model2.parent(), &model1);
}

TEST(StageModelTest, Init)
{
    StageModel model;
    ASSERT_EQ(model.stage(), nullptr);

    Stage stage;
    model.init(&stage);
    ASSERT_EQ(model.stage(), &stage);
}

TEST(StageModelTest, OnCostumeChanged)
{
    StageModel model;

    Costume costume("", "", "");

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateCostume(&costume));
    model.onCostumeChanged(&costume);
}

TEST(StageModelTest, OnGraphicsEffectChanged)
{
    StageModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    GraphicsEffect effect1(ShaderManager::Effect::Brightness, "brightness");
    EXPECT_CALL(renderedTarget, setGraphicEffect(ShaderManager::Effect::Brightness, 78.4));
    model.onGraphicsEffectChanged(&effect1, 78.4);

    GraphicsEffect effect2(ShaderManager::Effect::Ghost, "ghost");
    EXPECT_CALL(renderedTarget, setGraphicEffect(ShaderManager::Effect::Ghost, 0.0));
    model.onGraphicsEffectChanged(&effect2, 0.0);
}

TEST(StageModelTest, OnGraphicsEffectsCleared)
{
    StageModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, clearGraphicEffects());
    model.onGraphicsEffectsCleared();
}

TEST(StageModelTest, RenderedTarget)
{
    StageModel model;
    ASSERT_EQ(model.renderedTarget(), nullptr);
    Stage stage;
    model.init(&stage);

    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");
    auto c3 = std::make_shared<Costume>("", "", "");
    stage.addCostume(c1);
    stage.addCostume(c2);
    stage.addCostume(c3);
    stage.setCostumeIndex(1);

    RenderedTargetMock renderedTarget;
    QSignalSpy spy(&model, &StageModel::renderedTargetChanged);
    EXPECT_CALL(renderedTarget, updateCostume(c2.get()));
    model.setRenderedTarget(&renderedTarget);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.renderedTarget(), &renderedTarget);

    EXPECT_CALL(renderedTarget, updateCostume(c3.get()));
    stage.setCostumeIndex(2);
    model.loadCostume();
}
