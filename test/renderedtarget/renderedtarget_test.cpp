#include <QtTest/QSignalSpy>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <qnanopainter.h>
#include <renderedtarget.h>
#include <stagemodel.h>
#include <spritemodel.h>
#include <scenemousearea.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

class RenderedTargetTest : public testing::Test
{
    public:
        void createContextAndSurface(QOpenGLContext *context, QOffscreenSurface *surface)
        {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setMajorVersion(4);
            surfaceFormat.setMinorVersion(3);

            context->setFormat(surfaceFormat);
            context->create();
            ASSERT_TRUE(context->isValid());

            surface->setFormat(surfaceFormat);
            surface->create();
            ASSERT_TRUE(surface->isValid());

            context->makeCurrent(surface);
            ASSERT_EQ(QOpenGLContext::currentContext(), context);
        }
};

TEST_F(RenderedTargetTest, Constructors)
{
    QQuickItem item1;
    QQuickItem item2(&item1);
    ASSERT_EQ(item2.parent(), &item1);
    ASSERT_EQ(item2.parentItem(), &item1);
}

TEST_F(RenderedTargetTest, UpdateMethods)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);
    RenderedTarget parent; // a parent item is needed for setVisible() to work
    RenderedTarget target(&parent);
    EngineMock engine;
    target.setEngine(&engine);
    QSignalSpy mirrorHorizontallySpy(&target, &RenderedTarget::mirrorHorizontallyChanged);
    ASSERT_FALSE(target.costumesLoaded());

    // Stage
    Stage stage;
    StageModel stageModel;
    stage.setInterface(&stageModel);
    target.setStageModel(&stageModel);
    auto costume = std::make_shared<Costume>("", "", "png");
    std::string costumeData = readFileStr("image.png");
    costume->setData(costumeData.size(), static_cast<void *>(costumeData.data()));
    costume->setRotationCenterX(-23);
    costume->setRotationCenterY(72);
    costume->setBitmapResolution(2.5);
    stage.addCostume(costume);
    target.loadCostumes();
    ASSERT_TRUE(target.costumesLoaded());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateCostume(costume.get());
    target.beforeRedraw();
    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 263);
    ASSERT_EQ(target.y(), 108);
    ASSERT_EQ(target.z(), 0);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(-23, 72));
    ASSERT_EQ(target.transformOrigin(), QQuickItem::Center);
    ASSERT_EQ(target.scale(), 0.4);
    ASSERT_FALSE(target.smooth());

    target.setStageModel(nullptr);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    Texture texture = target.texture();
    ASSERT_TRUE(texture.isValid());
    ASSERT_EQ(texture.width(), 4);
    ASSERT_EQ(texture.height(), 6);

    // Sprite
    Sprite sprite;
    sprite.setVisible(true);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setDirection(-67.16);
    sprite.setSize(143.98);
    sprite.setX(0);
    sprite.setY(0);
    sprite.setLayerOrder(3);
    sprite.addCostume(costume);
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);

    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    target.setSpriteModel(&spriteModel);
    target.loadCostumes();
    target.beforeRedraw();

    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 263);
    ASSERT_EQ(target.y(), 108);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_EQ(target.transformOriginPoint().x(), -23);
    ASSERT_EQ(target.transformOriginPoint().y(), 72);
    ASSERT_EQ(target.transformOrigin(), QQuickItem::Center);
    ASSERT_EQ(std::round(target.scale() * 100) / 100, 0.58);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());
    ASSERT_FALSE(target.smooth());

    texture = target.texture();
    ASSERT_TRUE(texture.isValid());
    ASSERT_EQ(texture.width(), 4);
    ASSERT_EQ(texture.height(), 6);

    // Visibility
    target.updateVisibility(false);
    ASSERT_FALSE(target.isVisible());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateVisibility(true);
    ASSERT_TRUE(target.isVisible());

    // X
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateX(12.5);
    ASSERT_EQ(target.x(), 275.5);
    ASSERT_EQ(target.y(), 108);

    // Y
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateY(-76.09);
    ASSERT_EQ(target.x(), 275.5);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 184.09);

    // Size
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateSize(56.2);
    target.beforeRedraw();
    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 275.5);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 184.09);
    ASSERT_EQ(target.transformOriginPoint().x(), -23);
    ASSERT_EQ(target.transformOriginPoint().y(), 72);

    // Direction
    target.updateDirection(123.8);
    ASSERT_EQ(target.rotation(), 33.8);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    // Rotation style
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.x(), 275.5);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 184.09);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateDirection(-15);
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.x(), 229.5);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 184.09);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateDirection(134.89);
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.x(), 275.5);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 184.09);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    // Stage scale
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setStageScale(1.5);
    target.beforeRedraw();
    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 401.75);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 312.14);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint().x(), -23);
    ASSERT_EQ(target.transformOriginPoint().y(), 72);
    ASSERT_EQ(target.transformOrigin(), QQuickItem::Center);
    ASSERT_EQ(std::round(target.scale() * 100) / 100, 0.34);

    // Null rotation center
    costume->setRotationCenterX(0);
    costume->setRotationCenterY(0);
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateSize(100);
    target.beforeRedraw();
    ASSERT_EQ(target.width(), 4);
    ASSERT_EQ(target.height(), 6);
    ASSERT_EQ(target.x(), 378.75);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 384.14);
    ASSERT_EQ(target.transformOriginPoint().x(), 0);
    ASSERT_EQ(target.transformOriginPoint().y(), 0);
    ASSERT_EQ(target.transformOrigin(), QQuickItem::TopLeft);

    // SVG
    costume = std::make_shared<Costume>("", "", "svg");
    std::string svgCostumeData = readFileStr("image.svg");
    costume->setData(svgCostumeData.size(), static_cast<void *>(svgCostumeData.data()));
    costume->setRotationCenterX(25);
    costume->setRotationCenterY(-8);
    sprite.addCostume(costume);

    EXPECT_CALL(engine, stageWidth()).Times(2).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).Times(2).WillRepeatedly(Return(360));
    target.loadCostumes();
    target.updateCostume(costume.get());
    target.beforeRedraw();

    ASSERT_EQ(target.width(), 26);
    ASSERT_EQ(target.height(), 26);
    ASSERT_EQ(target.x(), 328.75);
    ASSERT_EQ(std::round(target.y() * 100) / 100, 400.14);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint().x(), 50);
    ASSERT_EQ(target.transformOriginPoint().y(), -16);
    ASSERT_EQ(target.transformOrigin(), QQuickItem::Center);
    ASSERT_EQ(std::round(target.scale() * 100) / 100, 0.75);
    ASSERT_TRUE(target.smooth());

    texture = target.texture();
    ASSERT_TRUE(texture.isValid());
    ASSERT_EQ(texture.width(), 26);
    ASSERT_EQ(texture.height(), 26);

    // Stage scale (SVG) - should update width and height
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setStageScale(3.5);
    ASSERT_EQ(target.width(), 52);
    ASSERT_EQ(target.height(), 52);

    context.doneCurrent();
}

TEST_F(RenderedTargetTest, DeinitClone)
{
    RenderedTarget target1, target2;
    SceneMouseArea mouseArea;
    target1.setMouseArea(&mouseArea);

    mouseArea.setDraggedSprite(&target2);
    target1.deinitClone();
    ASSERT_EQ(mouseArea.draggedSprite(), &target2);

    mouseArea.setDraggedSprite(&target1);
    target1.deinitClone();
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
}

TEST_F(RenderedTargetTest, HullPoints)
{
    EngineMock engine;
    Sprite sprite;
    SpriteModel model;
    model.init(&sprite);

    QQuickItem parent;
    parent.setWidth(480);
    parent.setHeight(360);
    RenderedTarget target(&parent);
    target.setEngine(&engine);
    target.setSpriteModel(&model);

    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Load costume
    EXPECT_CALL(engine, stageWidth()).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillRepeatedly(Return(360));
    auto costume = std::make_shared<Costume>("", "", "png");
    std::string costumeData = readFileStr("image.png");
    costume->setData(costumeData.size(), static_cast<void *>(costumeData.data()));
    sprite.addCostume(costume);
    target.loadCostumes();
    target.updateCostume(costume.get());
    target.setStageScale(2);
    target.setX(25);
    target.setY(30);

    // Test hull points
    target.setWidth(3);
    target.setHeight(3);
    ASSERT_EQ(target.hullPoints(), std::vector<QPoint>({ { 1, 1 }, { 2, 1 }, { 3, 1 }, { 1, 2 }, { 3, 2 }, { 1, 3 }, { 2, 3 }, { 3, 3 } }));

    // Test contains()
    ASSERT_FALSE(target.contains({ 0, 0 }));
    ASSERT_FALSE(target.contains({ 1, 0 }));
    ASSERT_FALSE(target.contains({ 2, 0 }));
    ASSERT_FALSE(target.contains({ 3, 0 }));

    ASSERT_FALSE(target.contains({ 0, 1 }));
    ASSERT_TRUE(target.contains({ 1, 1 }));
    ASSERT_TRUE(target.contains({ 1.4, 1.25 }));
    ASSERT_TRUE(target.contains({ 2, 1 }));
    ASSERT_TRUE(target.contains({ 3, 1 }));

    ASSERT_TRUE(target.contains({ 1, 2 }));
    ASSERT_FALSE(target.contains({ 2, 2 }));
    ASSERT_TRUE(target.contains({ 3, 2 }));
    ASSERT_FALSE(target.contains({ 3.5, 2.1 }));

    ASSERT_TRUE(target.contains({ 1, 3 }));
    ASSERT_TRUE(target.contains({ 2, 3 }));
    ASSERT_TRUE(target.contains({ 3, 3 }));
    ASSERT_FALSE(target.contains({ 3.3, 3.5 }));

    // Test contains() with horizontal mirroring
    target.updateRotationStyle(Sprite::RotationStyle::LeftRight);
    target.updateDirection(-45);
    target.setX(25);
    target.setY(30);
    ASSERT_FALSE(target.contains({ 0, 0 }));
    ASSERT_TRUE(target.contains({ -1, 1 }));
    ASSERT_FALSE(target.contains({ -2, 2 }));
    ASSERT_TRUE(target.contains({ -3, 2 }));
    ASSERT_FALSE(target.contains({ -3.5, 2.1 }));
    ASSERT_TRUE(target.contains({ -2, 3 }));
    ASSERT_FALSE(target.contains({ -3.3, 3.5 }));

    // Test containsScratchPoint()
    target.updateDirection(0);
    target.setX(25);
    target.setY(30);
    ASSERT_FALSE(target.containsScratchPoint(-227.5, 165)); // [0, 0]
    ASSERT_FALSE(target.containsScratchPoint(-226.5, 165)); // [1, 0]
    ASSERT_FALSE(target.containsScratchPoint(-225.5, 165)); // [2, 0]
    ASSERT_FALSE(target.containsScratchPoint(-224.5, 165)); // [3, 0]

    ASSERT_FALSE(target.containsScratchPoint(-227.5, 164));   // [0, 1]
    ASSERT_TRUE(target.containsScratchPoint(-226.5, 164));    // [1, 1]
    ASSERT_TRUE(target.containsScratchPoint(-226.1, 163.75)); // [1.4, 1.25]
    ASSERT_TRUE(target.containsScratchPoint(-225.5, 164));    // [2, 1]
    ASSERT_TRUE(target.containsScratchPoint(-224.5, 164));    // [3, 1]

    ASSERT_TRUE(target.containsScratchPoint(-226.5, 163));  // [1, 2]
    ASSERT_FALSE(target.containsScratchPoint(-225.5, 163)); // [2, 2]
    ASSERT_TRUE(target.containsScratchPoint(-224.5, 163));  // [3, 2]
    ASSERT_FALSE(target.containsScratchPoint(-224, 162.9)); // [3.5, 2.1]

    ASSERT_TRUE(target.containsScratchPoint(-226.5, 162));    // [1, 3]
    ASSERT_TRUE(target.containsScratchPoint(-225.5, 162));    // [2, 3]
    ASSERT_TRUE(target.containsScratchPoint(-224.5, 162));    // [3, 3]
    ASSERT_FALSE(target.containsScratchPoint(-224.2, 161.5)); // [3.3, 3.5]

    // Cleanup
    context.doneCurrent();
}

TEST_F(RenderedTargetTest, SpriteDragging)
{
    RenderedTarget target;
    EngineMock engine;
    target.setEngine(&engine);

    SpriteModel model;
    Sprite sprite;
    sprite.setX(64.08);
    sprite.setY(-6.86);
    model.init(&sprite);
    target.setSpriteModel(&model);
    target.setStageScale(3.5);

    SceneMouseArea mouseArea;
    target.setMouseArea(&mouseArea);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try right mouse button (should not work)
    QMouseEvent moveEventRightButton(QEvent::MouseMove, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QMouseEvent pressEventRightButton(QEvent::MouseButtonPress, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QMouseEvent releaseEventRightButton(QEvent::MouseButtonRelease, QPointF(), QPointF(), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &pressEventRightButton);
    QCoreApplication::sendEvent(&target, &moveEventRightButton);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEventRightButton);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try right mouse button with "draggable" set to true (should not work)
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &pressEventRightButton);
    QCoreApplication::sendEvent(&target, &moveEventRightButton);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &releaseEventRightButton);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    // Try left mouse button (should not work with "draggable" set to false)
    sprite.setDraggable(false);
    QMouseEvent moveEvent(QEvent::MouseMove, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(), QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&target, &pressEvent);
    QCoreApplication::sendEvent(&target, &moveEvent);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    emit mouseArea.mouseMoved(1064, 651);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEvent);

    // Try left mouse button with "draggable" set to true
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &pressEvent);
    EXPECT_CALL(engine, mouseX()).WillOnce(Return(67.95));
    EXPECT_CALL(engine, mouseY()).WillOnce(Return(2.1));
    EXPECT_CALL(engine, moveSpriteToFront(&sprite));
    QCoreApplication::sendEvent(&target, &moveEvent);
    ASSERT_EQ(sprite.x(), 64.08);
    ASSERT_EQ(sprite.y(), -6.86);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    // Drag
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    emit mouseArea.mouseMoved(1067.8, 649.06);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 61.22);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -14.41);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    emit mouseArea.mouseMoved(1092.47, 605.46);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 68.26);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -1.95);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);

    // Create another sprite
    RenderedTarget anotherTarget;
    anotherTarget.setEngine(&engine);

    SpriteModel anotherModel;
    Sprite anotherSprite;
    anotherSprite.setX(64.08);
    anotherSprite.setY(-6.86);
    anotherSprite.setDraggable(true);
    anotherModel.init(&anotherSprite);
    anotherTarget.setSpriteModel(&model);
    anotherTarget.setStageScale(3.5);
    anotherTarget.setMouseArea(&mouseArea);

    // Try to drag the second sprite while the first is being dragged
    sprite.setDraggable(true);
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&anotherTarget, &pressEvent);
    QCoreApplication::sendEvent(&anotherTarget, &moveEvent);
    ASSERT_EQ(mouseArea.draggedSprite(), &target);
    EXPECT_CALL(engine, clickTarget(&sprite));
    QCoreApplication::sendEvent(&anotherTarget, &releaseEvent);

    // Stop dragging
    EXPECT_CALL(engine, clickTarget).Times(0);
    QCoreApplication::sendEvent(&target, &releaseEvent);
    ASSERT_EQ(std::round(sprite.x() * 100) / 100, 68.26);
    ASSERT_EQ(std::round(sprite.y() * 100) / 100, -1.95);
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);
}

TEST_F(RenderedTargetTest, Engine)
{
    RenderedTarget target;
    ASSERT_EQ(target.engine(), nullptr);

    EngineMock engine;
    target.setEngine(&engine);
    ASSERT_EQ(target.engine(), &engine);
}

TEST_F(RenderedTargetTest, StageModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.stageModel(), nullptr);

    StageModel model;
    target.setStageModel(&model);
    ASSERT_EQ(target.stageModel(), &model);
}

TEST_F(RenderedTargetTest, SpriteModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.spriteModel(), nullptr);

    SpriteModel model;
    target.setSpriteModel(&model);
    ASSERT_EQ(target.spriteModel(), &model);
}

TEST_F(RenderedTargetTest, ScratchTarget)
{
    RenderedTarget target;
    ASSERT_EQ(target.scratchTarget(), nullptr);

    StageModel stageModel;
    Stage stage;
    stageModel.init(&stage);
    target.setStageModel(&stageModel);
    ASSERT_EQ(target.scratchTarget(), &stage);

    target.setStageModel(nullptr);
    SpriteModel spriteModel;
    Sprite sprite;
    spriteModel.init(&sprite);
    target.setSpriteModel(&spriteModel);
    ASSERT_EQ(target.scratchTarget(), &sprite);
}

TEST_F(RenderedTargetTest, MouseArea)
{
    RenderedTarget target;
    ASSERT_EQ(target.mouseArea(), nullptr);

    SceneMouseArea mouseArea;
    target.setMouseArea(&mouseArea);
    ASSERT_EQ(target.mouseArea(), &mouseArea);
}

TEST_F(RenderedTargetTest, StageScale)
{
    RenderedTarget target;
    ASSERT_EQ(target.stageScale(), 1);

    target.setStageScale(6.4);
    ASSERT_EQ(target.stageScale(), 6.4);
}

TEST_F(RenderedTargetTest, GraphicEffects)
{
    RenderedTarget target;
    ASSERT_TRUE(target.graphicEffects().empty());

    target.setGraphicEffect(ShaderManager::Effect::Color, 23.5);
    target.setGraphicEffect(ShaderManager::Effect::Ghost, 95.7);
    std::unordered_map<ShaderManager::Effect, double> expected;
    expected[ShaderManager::Effect::Color] = 23.5;
    expected[ShaderManager::Effect::Ghost] = 95.7;
    ASSERT_EQ(target.graphicEffects(), expected);

    target.setGraphicEffect(ShaderManager::Effect::Color, 0);
    expected.erase(ShaderManager::Effect::Color);
    ASSERT_EQ(target.graphicEffects(), expected);

    target.setGraphicEffect(ShaderManager::Effect::Ghost, 0.5);
    expected[ShaderManager::Effect::Ghost] = 0.5;
    ASSERT_EQ(target.graphicEffects(), expected);

    target.setGraphicEffect(ShaderManager::Effect::Brightness, -150.7);
    expected[ShaderManager::Effect::Brightness] = -150.7;
    ASSERT_EQ(target.graphicEffects(), expected);

    target.clearGraphicEffects();
    ASSERT_TRUE(target.graphicEffects().empty());
}

TEST_F(RenderedTargetTest, GetBounds)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);
    RenderedTarget target;

    Sprite sprite;
    sprite.setX(75.64);
    sprite.setY(-120.3);
    sprite.setDirection(-46.37);
    sprite.setSize(67.98);
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);
    target.setSpriteModel(&spriteModel);
    EngineMock engine;
    target.setEngine(&engine);
    auto costume = std::make_shared<Costume>("", "", "png");
    std::string costumeData = readFileStr("image.png");
    costume->setData(costumeData.size(), static_cast<void *>(costumeData.data()));
    costume->setRotationCenterX(-15);
    costume->setRotationCenterY(48);
    costume->setBitmapResolution(3.25);
    sprite.addCostume(costume);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.loadCostumes();
    target.updateCostume(costume.get());
    target.beforeRedraw();

    Rect bounds = target.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 66.13);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -124.52);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 66.72);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -125.11);

    QRectF bubbleBounds = target.getBoundsForBubble();
    ASSERT_EQ(std::round(bubbleBounds.left() * 100) / 100, 66.13);
    ASSERT_EQ(std::round(bubbleBounds.top() * 100) / 100, -124.52);
    ASSERT_EQ(std::round(bubbleBounds.right() * 100) / 100, 66.72);
    ASSERT_EQ(std::round(bubbleBounds.bottom() * 100) / 100, -125.11);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::LeftRight);

    bounds = target.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 71.87);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -110.47);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 72.29);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -110.89);

    bubbleBounds = target.getBoundsForBubble();
    ASSERT_EQ(std::round(bubbleBounds.left() * 100) / 100, 71.87);
    ASSERT_EQ(std::round(bubbleBounds.top() * 100) / 100, -110.47);
    ASSERT_EQ(std::round(bubbleBounds.right() * 100) / 100, 72.29);
    ASSERT_EQ(std::round(bubbleBounds.bottom() * 100) / 100, -110.89);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setStageScale(20.75);

    bounds = target.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 71.87);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -110.47);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 72.29);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -110.89);

    bubbleBounds = target.getBoundsForBubble();
    ASSERT_EQ(std::round(bubbleBounds.left() * 100) / 100, 71.87);
    ASSERT_EQ(std::round(bubbleBounds.top() * 100) / 100, -110.47);
    ASSERT_EQ(std::round(bubbleBounds.right() * 100) / 100, 72.29);
    ASSERT_EQ(std::round(bubbleBounds.bottom() * 100) / 100, -110.89);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateSize(9780.6);

    bounds = target.getBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, -466.05);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, 1294.13);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, -405.87);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, 1233.94);

    bubbleBounds = target.getBoundsForBubble();
    ASSERT_EQ(std::round(bubbleBounds.left() * 100) / 100, -466.05);
    ASSERT_EQ(std::round(bubbleBounds.top() * 100) / 100, 1294.13);
    ASSERT_EQ(std::round(bubbleBounds.right() * 100) / 100, -405.87);
    ASSERT_EQ(std::round(bubbleBounds.bottom() * 100) / 100, 1286.13);

    context.doneCurrent();
}

TEST_F(RenderedTargetTest, GetFastBounds)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);
    QOpenGLExtraFunctions glF(&context);
    glF.initializeOpenGLFunctions();
    RenderedTarget target;

    Sprite sprite;
    sprite.setX(75.64);
    sprite.setY(-120.3);
    sprite.setDirection(-46.37);
    sprite.setSize(67.98);
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);
    target.setSpriteModel(&spriteModel);
    EngineMock engine;
    target.setEngine(&engine);
    auto costume = std::make_shared<Costume>("", "", "png");
    std::string costumeData = readFileStr("image.png");
    costume->setData(costumeData.size(), static_cast<void *>(costumeData.data()));
    costume->setRotationCenterX(-15);
    costume->setRotationCenterY(48);
    costume->setBitmapResolution(3.25);
    sprite.addCostume(costume);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.loadCostumes();
    target.updateCostume(costume.get());
    target.beforeRedraw();

    Rect bounds = target.getFastBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 65.84);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -123.92);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 67.31);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -125.4);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateRotationStyle(Sprite::RotationStyle::LeftRight);

    bounds = target.getFastBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 71.67);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -110.26);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 72.5);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -111.51);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.setStageScale(20.75);

    bounds = target.getFastBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, 71.67);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, -110.26);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, 72.5);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, -111.51);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    target.updateSize(9780.6);

    bounds = target.getFastBounds();
    ASSERT_EQ(std::round(bounds.left() * 100) / 100, -496.15);
    ASSERT_EQ(std::round(bounds.top() * 100) / 100, 1324.22);
    ASSERT_EQ(std::round(bounds.right() * 100) / 100, -375.77);
    ASSERT_EQ(std::round(bounds.bottom() * 100) / 100, 1143.65);

    context.doneCurrent();
}
