#ifndef GRAPHICSSCENE3D_H
#define GRAPHICSSCENE3D_H

#include <coordinateaxes.h>
#include <planegrid.h>
#include <bottomtrack.h>
#include <surface.h>
#include <pointgroup.h>
#include <polygongroup.h>
#include <graphicsscene3dview.h>

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QVector2D>
#include <QMutex>

#include <memory>

class QOpenGLShaderProgram;
class GraphicsScene3dRenderer : protected QOpenGLFunctions
{
public:
    GraphicsScene3dRenderer();
    virtual ~GraphicsScene3dRenderer();
    void render();

private:
    void initialize();
    void drawObjects();
    QMatrix4x4 model() const;
    QMatrix4x4 view() const;
    QMatrix4x4 projection() const;

protected:

    QMap <QString, std::shared_ptr <QOpenGLShaderProgram>> m_shaderProgramMap;
    bool m_isInitialized = false;

private:
    friend class GraphicsScene3dView;
    friend class InFboRenderer;

    QSizeF m_viewSize;
    GraphicsScene3dView::Camera m_camera;
    GraphicsScene3dView::Camera m_axesThumbnailCamera;
    CoordinateAxes::CoordinateAxesRenderImplementation m_coordAxesRenderImpl;
    PlaneGrid::PlaneGridRenderImplementation m_planeGridRenderImpl;
    Surface::SurfaceRenderImplementation m_surfaceRenderImpl;
    BottomTrack::BottomTrackRenderImplementation m_bottomTrackRenderImpl;
    PolygonGroup::PolygonGroupRenderImplementation m_polygonGroupRenderImpl;
    PointGroup::PointGroupRenderImplementation m_pointGroupRenderImpl;

    QRectF m_rect;
    QVector3D m_dragOffset;
    QVector3D m_rotationAngle;
    QVector3D m_center;
    qreal m_fov = 45.0f;
    qreal m_pitch = 0.0f;
    qreal m_yaw = -90.0f;
};

#endif // GRAPHICSSCENE3D_H