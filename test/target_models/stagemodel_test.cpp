#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <scratchcpp/value.h>
#include <stagemodel.h>
#include <graphicseffect.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

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

TEST(StageModelTest, OnBubbleTypeChanged)
{
    StageModel model;
    QSignalSpy spy(&model, &StageModel::bubbleTypeChanged);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);

    model.onBubbleTypeChanged(Target::BubbleType::Think);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    model.onBubbleTypeChanged(Target::BubbleType::Think);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    model.onBubbleTypeChanged(Target::BubbleType::Say);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);
    ASSERT_EQ(spy.count(), 2);

    model.onBubbleTypeChanged(Target::BubbleType::Say);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);
    ASSERT_EQ(spy.count(), 2);
}

TEST(StageModelTest, OnBubbleTextChanged)
{
    StageModel model;
    QSignalSpy spy(&model, &StageModel::bubbleTextChanged);
    ASSERT_TRUE(model.bubbleText().isEmpty());

    model.onBubbleTextChanged("Hello!");
    ASSERT_EQ(model.bubbleText(), "Hello!");
    ASSERT_EQ(spy.count(), 1);

    model.onBubbleTextChanged("Hello!");
    ASSERT_EQ(model.bubbleText(), "Hello!");
    ASSERT_EQ(spy.count(), 1);

    model.onBubbleTextChanged("test");
    ASSERT_EQ(model.bubbleText(), "test");
    ASSERT_EQ(spy.count(), 2);
}

TEST(StageModelTest, CostumeWidth)
{
    StageModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, costumeWidth()).WillOnce(Return(15));
    ASSERT_EQ(model.costumeWidth(), 15);
}

TEST(StageModelTest, CostumeHeight)
{
    StageModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, costumeHeight()).WillOnce(Return(10));
    ASSERT_EQ(model.costumeHeight(), 10);
}

TEST(SpriteModelTest, TouchingClones)
{
    StageModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Sprite clone1, clone2;
    std::vector<Sprite *> clones = { &clone1, &clone2 };
    std::vector<Sprite *> actualClones;

    EXPECT_CALL(renderedTarget, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(model.touchingClones(clones));
    ASSERT_EQ(actualClones, clones);

    EXPECT_CALL(renderedTarget, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return true;
    })));
    ASSERT_TRUE(model.touchingClones(clones));
    ASSERT_EQ(actualClones, clones);
}

TEST(StageModelTest, TouchingPoint)
{
    StageModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, containsScratchPoint(56.3, -179.4)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingPoint(56.3, -179.4));

    EXPECT_CALL(renderedTarget, containsScratchPoint(-20.08, 109.47)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingPoint(-20.08, 109.47));
}

TEST(StageModelTest, TouchingColor)
{
    StageModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Value color1 = 123, color2 = 456;
    EXPECT_CALL(renderedTarget, touchingColor(color1)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingColor(color1));

    EXPECT_CALL(renderedTarget, touchingColor(color1)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingColor(color1));

    EXPECT_CALL(renderedTarget, touchingColor(color1, color2)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingColor(color1, color2));

    EXPECT_CALL(renderedTarget, touchingColor(color1, color2)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingColor(color1, color2));
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
    model.setRenderedTarget(&renderedTarget);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.renderedTarget(), &renderedTarget);

    EXPECT_CALL(renderedTarget, updateCostume(c3.get()));
    stage.setCostumeIndex(2);
    model.loadCostume();
}
