#include "3Plot.h"
#include <bottomtrack.h>
#include <surface.h>

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QRandomGenerator>

#include <iostream>

#include <polygonobject.h>

class FboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    FboRenderer()
    {
        scene.initialize();
    }

    void render() override {
        scene.render();
        update();
    }

    //void setSceneObjectsListModel(std::shared_ptr <SceneObjectsListModel> sceneObjectsListModel){
    //    //scene.setSceneObjectsListModel(sceneObjectsListModel);
    //}

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    void synchronize(QQuickFramebufferObject *item) override {
        FboInSGRenderer *fbitem = static_cast<FboInSGRenderer *>(item);
        //scene.setLines(fbitem->lines());

        scene.scale(fbitem->scaleDelta());
<<<<<<< HEAD
        //scene.size(fbitem->size());
        scene.mouse(fbitem->mouse());
        scene.rotationFlag(fbitem->isRotation());
        //scene.setRightMouseButtonPressed(fbitem->isRightMouseButtonPressed());
        //scene.setMousePos(fbitem->mousePos());
        //scene.setSize(QSize(fbitem->width(), fbitem->height()));
=======
        scene.modelScale(fbitem->modelScaleZ());
        scene.size(fbitem->size());
        scene.mouse(fbitem->mouse());
        scene.rotationFlag(fbitem->isRotation());
>>>>>>> 342fff10a8cd1645a2955a8d2d881b1ebd45ef44

    }

    Scene3D scene;
};

QQuickFramebufferObject::Renderer *FboInSGRenderer::createRenderer() const
{
    auto renderer = new FboRenderer();

    return renderer;
}

#include <QPainter>
#include <QPaintEngine>
#include <qmath.h>
#include <QtOpenGL/QtOpenGL>
#include <memory>

Scene3D::Scene3D()
:mpStaticColorShaderProgram(new QOpenGLShaderProgram)
,mpHeightColorShaderProgram(new QOpenGLShaderProgram)
{
    mShaderProgramMap["height"] = mpHeightColorShaderProgram.get();
    mShaderProgramMap["static"] = mpStaticColorShaderProgram.get();
}

Scene3D::~Scene3D()
{
}

<<<<<<< HEAD
//void Scene3D::setSceneObjectsListModel(std::shared_ptr <SceneObjectsListModel> sceneObjectsListModel)
//{
//    mpSceneObjectsListModel = sceneObjectsListModel;
//}
=======
void Scene3D::setModel(const ModelPointer pModel)
{
    mpModel = pModel;

    if (!mpModel) return;

    connect(mpModel.get(), &Q3DSceneModel::stateChanged,
            this         , &Scene3D::modelStateChanged);

    connect(mpModel.get(), &Q3DSceneModel::bottomTrackDataChanged,
            this         , &Scene3D::bottomTrackDataChanged);

    connect(mpModel.get(), &Q3DSceneModel::bottomTrackPropertiesChanged,
            this         , &Scene3D::bottomTrackPropertiesChanged);

    connect(mpModel.get(), &Q3DSceneModel::surfaceDataChanged,
            this         , &Scene3D::surfaceDataChanged);

    connect(mpModel.get(), &Q3DSceneModel::surfacePropertiesChanged,
            this         , &Scene3D::surfacePropertiesChanged);

    connect(mpModel.get(), &Q3DSceneModel::contourDataChanged,
            this         , &Scene3D::contourDataChanged);

    connect(mpModel.get(), &Q3DSceneModel::contourPropertiesChanged,
            this         , &Scene3D::contourPropertiesChanged);

    auto object = mpModel->bottomTrackDisplayedObject();

    mBottomTrackDisplayedObject.setData(object.cdata());
    mBottomTrackDisplayedObject.setPrimitiveType(object.primitiveType());

    bottomTrackPropertiesChanged();
    surfacePropertiesChanged();
    contourPropertiesChanged();
}

void Scene3D::modelStateChanged()
{
    //mpBottomTrackDisplayedObject = mpModel->bottomTrack();
   // mpSurfaceDisplayedObject = mpModel->surface();
    //mBottomTrack = mpModel->bottomTrack();

    //if (mBottomTrack.isEmpty())
    //    return;

    //mMaxZ = mBottomTrack.first().z();
    //mMinZ = mMaxZ;

    //for (const auto& p : mBottomTrack){
    //    mMaxZ = std::max(mMaxZ, p.z());
    //    mMinZ = std::min(mMinZ, p.z());
    //}

    //mBasicSurface = mpModel->basicSurface();
    //mSmoothedSurface = mpModel->smoothedSurface();
}

void Scene3D::bottomTrackDataChanged()
{
    auto object = mpModel->bottomTrackDisplayedObject();

    mBottomTrackDisplayedObject.setData(object.cdata());
}

void Scene3D::bottomTrackPropertiesChanged()
{
    auto object = mpModel->bottomTrackDisplayedObject();

    // TODO: Скопировать остальные свойства

    mBottomTrackDisplayedObject.setVisible(object.isVisible());
}

void Scene3D::surfaceDataChanged()
{
    auto object = mpModel->surfaceDisplayedObject();

    mSurfaceDisplayedObject.setData(object.cdata());
    mSurfaceDisplayedObject.setPrimitiveType(object.primitiveType());
}

void Scene3D::surfacePropertiesChanged()
{
    auto object = mpModel->surfaceDisplayedObject();

    mSurfaceDisplayedObject.setVisible(object.isVisible());
    mSurfaceDisplayedObject.setGridVisible(object.isGridVisible());
}

void Scene3D::contourDataChanged()
{
    auto object = mpModel->contourDisplayedObject();

    mContourDisplayedObject.setPrimitiveType(object.primitiveType());
    mContourDisplayedObject.setData(object.cdata());
}

void Scene3D::contourPropertiesChanged()
{
    auto object = mpModel->contourDisplayedObject();

    mContourDisplayedObject.setVisible(object.isVisible());
    mContourDisplayedObject.setColor(object.rgbColor());
    mContourDisplayedObject.setLineWidth(object.lineWidth());
    mContourDisplayedObject.setKeyPointsVisible(object.keyPointsVisible());
    mContourDisplayedObject.setKeyPointsColor(object.keyPointsRgbColor());
}
>>>>>>> 342fff10a8cd1645a2955a8d2d881b1ebd45ef44

void Scene3D::paintScene()
{
    //for(const auto& object : m_objectList)
    //    object->draw(this, m_projection * m_view * m_model, m_shaderProgramMap);

<<<<<<< HEAD
    if (!mpStaticColorShaderProgram->bind()){
=======
    if (mSurfaceDisplayedObject.isVisible()){
        displayBottomSurface();
    }

    if (mSurfaceDisplayedObject.isGridVisible()){
        displayBottomSurfaceGrid();
    }

    if (mContourDisplayedObject.isVisible()){
        displayContour();
    }

    if (mContourDisplayedObject.keyPointsVisible()){
        displayContourKeyPoints();
    }

    displayTestPoints();
}

void Scene3D::displayTestPoints() {

    auto pProgram = mpHeightColorShaderProgram.get();

    if (mSurfaceDisplayedObject.isVisible() ||
        mSurfaceDisplayedObject.isGridVisible()){
        pProgram = mpStaticColorShaderProgram.get();
    }

    if (!pProgram->bind()){
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc    = pProgram->attributeLocation("position");
    int maxZLoc   = pProgram->uniformLocation("max_z");
    int minZLoc   = pProgram->uniformLocation("min_z");
    int matrixLoc = pProgram->uniformLocation("matrix");

    QVector4D color(0.8f, 0.2f, 0.7f, 1.0f);
    int colorLoc = pProgram->uniformLocation("color");


    pProgram->setUniformValue(colorLoc,color);
    pProgram->setUniformValue(maxZLoc, mBottomTrackDisplayedObject.maximumZ());
    pProgram->setUniformValue(minZLoc, mBottomTrackDisplayedObject.minimumZ());
    pProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);
    pProgram->enableAttributeArray(posLoc);
    pProgram->setAttributeArray(posLoc, _testPonts.constData());
#if !defined(Q_OS_ANDROID)
    glPointSize(10);
#endif
    glDrawArrays(GL_POINTS, 0, _testPonts.size());
    pProgram->disableAttributeArray(posLoc);
    pProgram->release();
#if !defined(Q_OS_ANDROID)
    glPointSize(1);
#endif

    pProgram->release();
}

void Scene3D::displayBottomTrack() {

    auto pProgram = mpHeightColorShaderProgram.get();

    if (mSurfaceDisplayedObject.isVisible() ||
        mSurfaceDisplayedObject.isGridVisible()){
        pProgram = mpStaticColorShaderProgram.get();
    }

    if (!pProgram->bind()){
>>>>>>> 342fff10a8cd1645a2955a8d2d881b1ebd45ef44
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc    = mpStaticColorShaderProgram->attributeLocation("position");
    int matrixLoc = mpStaticColorShaderProgram->uniformLocation("matrix");
    int colorLoc  = mpStaticColorShaderProgram->uniformLocation("color");

    QVector4D color(0.8f, 1.0f, 0.7f, 0.0f);

    QVector <QVector3D> data{
        {0.0f, 2.0f, 12.0f},
        {10.0f, 2.0f, 12.0f},
        {18.0f, 12.0f, 22.0f},
        {15.0f, 62.0f, 12.0f},
    };

    //qDebug() << "p: " << m_projection <<", v: " << m_view << ", m: " << m_model;

    mpStaticColorShaderProgram->setUniformValue(colorLoc,color);
    mpStaticColorShaderProgram->setUniformValue(matrixLoc, mProjection * mView * mModel);
    mpStaticColorShaderProgram->enableAttributeArray(posLoc);
    mpStaticColorShaderProgram->setAttributeArray(posLoc, data.constData());

    glDrawArrays(GL_POLYGON, 0, data.size());

    mpStaticColorShaderProgram->disableAttributeArray(posLoc);
    mpStaticColorShaderProgram->release();
}

<<<<<<< HEAD
=======
void Scene3D::displayBottomSurface()
{
    auto pProgram = mpHeightColorShaderProgram.get();

    if (!pProgram->bind()){
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc    = pProgram->attributeLocation("position");
    int maxZLoc   = pProgram->uniformLocation("max_z");
    int minZLoc   = pProgram->uniformLocation("min_z");
    int matrixLoc = pProgram->uniformLocation("matrix");

    pProgram->setUniformValue(maxZLoc, mSurfaceDisplayedObject.maximumZ());
    pProgram->setUniformValue(minZLoc, mSurfaceDisplayedObject.minimumZ());
    pProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);

    pProgram->enableAttributeArray(posLoc);
    pProgram->setAttributeArray(posLoc, mSurfaceDisplayedObject.cdata().constData());
    glDrawArrays(mSurfaceDisplayedObject.primitiveType(), 0, mSurfaceDisplayedObject.cdata().size());
    //qDebug() <<"Type --> " << mSurfaceDisplayedObject.primitiveType();
    pProgram->disableAttributeArray(posLoc);
    pProgram->release();
}

void Scene3D::displayBottomSurfaceGrid()
{
    auto pProgram = mpHeightColorShaderProgram.get();

    if (mSurfaceDisplayedObject.isVisible()){
        pProgram = mpStaticColorShaderProgram.get();
    }

    if (!pProgram->bind()){
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc    = pProgram->attributeLocation("position");
    int maxZLoc   = pProgram->uniformLocation("max_z");
    int minZLoc   = pProgram->uniformLocation("min_z");
    int matrixLoc = pProgram->uniformLocation("matrix");

    if (mSurfaceDisplayedObject.isGridVisible()){
        QVector4D color(0.0f, 0.0f, 0.0f, 1.0f);
        int colorLoc = pProgram->uniformLocation("color");

        pProgram->setUniformValue(colorLoc, color);
    }

    pProgram->setUniformValue(maxZLoc, mSurfaceDisplayedObject.maximumZ());
    pProgram->setUniformValue(minZLoc, mSurfaceDisplayedObject.minimumZ());
    pProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);

    pProgram->enableAttributeArray(posLoc);
    pProgram->setAttributeArray(posLoc, mSurfaceDisplayedObject.cgrid().constData());
    glLineWidth(1.0);
    glDrawArrays(GL_LINES, 0, mSurfaceDisplayedObject.cgrid().size());
    pProgram->disableAttributeArray(posLoc);
    pProgram->release();
}

void Scene3D::displayContour()
{
    auto pProgram = mpStaticColorShaderProgram.get();

    if (!pProgram->bind()){
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc = pProgram->attributeLocation("position");
    int matrixLoc = pProgram->uniformLocation("matrix");

    QVector4D color = mContourDisplayedObject.color();

    int colorLoc = pProgram->uniformLocation("color");

    pProgram->setUniformValue(colorLoc, color);
    pProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);
    pProgram->enableAttributeArray(posLoc);
    pProgram->setAttributeArray(posLoc, mContourDisplayedObject.cdata().constData());

    glLineWidth(mContourDisplayedObject.lineWidth());
    glDrawArrays(mContourDisplayedObject.primitiveType(), 0, mContourDisplayedObject.cdata().size());
    glLineWidth(1.0f);

    pProgram->disableAttributeArray(posLoc);
    pProgram->release();
}

void Scene3D::displayContourKeyPoints()
{
    auto pProgram = mpStaticColorShaderProgram.get();

    if (!pProgram->bind()){
        qCritical() << "Error binding shader program.";
        return;
    }

    int posLoc = pProgram->attributeLocation("position");
    int matrixLoc = pProgram->uniformLocation("matrix");

    QVector4D color = mContourDisplayedObject.keyPointsColor();

    int colorLoc = pProgram->uniformLocation("color");

    pProgram->setUniformValue(colorLoc, color);
    pProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);
    pProgram->enableAttributeArray(posLoc);
    pProgram->setAttributeArray(posLoc, mContourDisplayedObject.cdata().constData());
#if !defined(Q_OS_ANDROID)
    glPointSize(mContourDisplayedObject.lineWidth());
#endif
    glDrawArrays(GL_POINTS, 0, mContourDisplayedObject.cdata().size());
#if !defined(Q_OS_ANDROID)
    glPointSize(1.0);
#endif

    pProgram->disableAttributeArray(posLoc);
    pProgram->release();
}

void FboInSGRenderer::setModel(const ModelPointer pModel)
{
    mpModel = pModel;
}


>>>>>>> 342fff10a8cd1645a2955a8d2d881b1ebd45ef44
void Scene3D::initialize()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);;
    glClearColor(0.1f, 0.1f, 0.2f, 0.0f);

    // ---------
    bool success = mpStaticColorShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/base.vsh");

    if (!success) qCritical() << "Error adding vertex shader from source file.";

    success = mpStaticColorShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/staticcolor.fsh");

    if (!success) qCritical() << "Error adding fragment shader from source file.";

    success = mpStaticColorShaderProgram->link();

    if (!success) qCritical() << "Error linking shaders in shader program.";

    // --------

    success = mpHeightColorShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/base.vsh");

    if (!success) qCritical() << "Error adding vertex shader from source file.";

    success = mpHeightColorShaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/heightcolor.fsh");

    if (!success) qCritical() << "Error adding fragment shader from source file.";

    success = mpHeightColorShaderProgram->link();

    if (!success) qCritical() << "Error linking shaders in shader program.";
}

void Scene3D::render()
{
    glDepthMask(true);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
//    glCullFace(GL_FRONT);
//    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    qreal zNear = 1, zFar = 5000.0, fov = m_fScale;

    mProjection = QMatrix4x4();
    mProjection.perspective(fov, _size.x()/_size.y(), zNear, zFar);

    if((_lastMouse.x() != _mouse.x() || _lastMouse.y() != _mouse.y()) && _mouse.x() >= 0 && _lastMouse.x() >= 0) {
        if(_isRotation) {
            _rotAngle += (_lastMouse - _mouse)*0.002;
            if(_rotAngle[1] > 1.570796f ) {
                _rotAngle[1] = 1.570796f;
            } else if(_rotAngle[1] < 0) {
                _rotAngle[1] = 0;
            }
        } else {
            QVector3D vm = QVector3D(-(_lastMouse.x() - _mouse.x()), (_lastMouse.y() - _mouse.y()), 0)*(m_fScale*0.02);

            _posCenter[0] += (vm[1]*cosf(-_rotAngle.x())*cosf(_rotAngle.y()) - vm[0]*sinf(-_rotAngle.x()));
            _posCenter[1] += (vm[1]*sinf(-_rotAngle.x())*cosf(_rotAngle.y()) + vm[0]*cosf(-_rotAngle.x()));
            _posCenter[2] += -vm[1]*sinf(_rotAngle.y())*sinf(_rotAngle.y());
        }
    }



    mView = QMatrix4x4();
    float r = -500.0;

    QVector3D cf;
    cf[0] = -sinf(_rotAngle.y())*cosf(-_rotAngle.x())*r;
    cf[1] = -sinf(_rotAngle.y())*sinf(-_rotAngle.x())*r;
    cf[2] = -cosf(_rotAngle.y())*r;

    QVector3D cu;
    cu[0] = cosf(_rotAngle.y())*cosf(-_rotAngle.x());
    cu[1] = cosf(_rotAngle.y())*sinf(-_rotAngle.x());
    cu[2] = -sinf(_rotAngle.y());

    mView.lookAt(cf + _posCenter, _posCenter, cu.normalized());

    mModel = QMatrix4x4();
    mModel.scale(1.0, 1.0, _modelScaleZ);

    if(_mouse.x() >= 0) {
        QVector3D zero(0, 0, 0);
        zero = zero.project(mView, mProjection, QRect(0, 0, _size.x(), _size.y()));
        QVector3D screenCoordinates(_mouse.x(), _mouse.y(), zero.z());
        QVector3D screenCoordinates0(_mouse.x(), _mouse.y(), 0);

        QVector3D unprj = screenCoordinates.unproject(mView, mProjection, QRect(0, 0, _size.x(), _size.y()));
//        QVector3D unprj0 = screenCoordinates0.unproject(mView, mProjection, QRect(0, 0, _size.x(), _size.y()));
//        QVector3D dir = (-unprj + unprj0);
        QVector3D ray = (-unprj + cf + _posCenter);

        float t = QVector3D::dotProduct(unprj - QVector3D(0,0,0), QVector3D(0,0,1))/QVector3D::dotProduct(ray, QVector3D(0,0,1));
        QVector3D p = unprj - t*ray;

//        _testPonts.append(p);
    }

    _lastMouse[0] = _mouse.x();
    _lastMouse[1] = _mouse.y();

    //auto pProgram = mBottomTrackDisplayedObject.shaderProgram();

    //// Настройка шейдерной программы
    //if (!pProgram || !pProgram->bind())
    //    return;


    //pProgram->release();

    //// Настройка шейдерной программы
    //mpOverlappedTrackProgram->bind();
    //QMatrix4x4 model = mModel;
    //model.translate(QVector3D(0.0f, 0.0f, 0.08f));
    //matrixLoc = mpOverlappedTrackProgram->uniformLocation("matrix");
    //mpOverlappedTrackProgram->setUniformValue(matrixLoc, mProjection*mView*mModel);
    //mpOverlappedTrackProgram->release();

    //// Настройка шейдерной программы
    //mpOverlappedGridProgram->bind();
    //matrixLoc = mpOverlappedGridProgram->uniformLocation("matrix");
    //// Приподнимаем сетку над поверхностью для корректной видимости
    //model = mModel;
    //model.translate(QVector3D(0.0f, 0.0f, 0.04f));
    //mpOverlappedGridProgram->setUniformValue(matrixLoc, mProjection*mView*model);
    //mpOverlappedGridProgram->release();

    paintScene();



    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}
