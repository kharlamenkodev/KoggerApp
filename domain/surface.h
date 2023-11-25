#ifndef SURFACE_H
#define SURFACE_H

#include <memory>

#include <sceneobject.h>
#include <contour.h>
#include <surfacegrid.h>
#include <surfaceprocessor.h>

class Surface : public SceneObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Surface)
    Q_PROPERTY(Contour* contour  READ contour CONSTANT)
    Q_PROPERTY(SurfaceGrid* grid READ grid    CONSTANT)
    Q_PROPERTY(SurfaceProcessorTask processingTask READ processingTask CONSTANT)
    Q_PROPERTY(float verticalScale READ verticalScale WRITE setVerticalScale)

public:
    class SurfaceRenderImplementation : public SceneObject::RenderImplementation
    {
    public:
        virtual void render(QOpenGLFunctions* ctx,
                          const QMatrix4x4& mvp,
                          const QMap <QString, std::shared_ptr <QOpenGLShaderProgram>>& shaderProgramMap) const override;

        virtual void render(QOpenGLFunctions* ctx,
                            const QMatrix4x4& model,
                            const QMatrix4x4& view,
                            const QMatrix4x4& projection,
                            const QMap <QString, std::shared_ptr <QOpenGLShaderProgram>>& shaderProgramMap) const;

    private:
        friend class Surface;
        SceneObject::RenderImplementation m_gridRenderImpl;
        SceneObject::RenderImplementation m_contourRenderImpl;
        float m_verticalScale = 1;
    };

    explicit Surface(QObject* parent = nullptr);
    virtual ~Surface();
    void setProcessingTask(const SurfaceProcessorTask& task);
    virtual void setData(const QVector<QVector3D>& data, int primitiveType = GL_POINTS) override;
    virtual void clearData() override;
    virtual SceneObjectType type() const override;
    Contour* contour() const;
    SurfaceGrid* grid() const;
    SurfaceProcessorTask processingTask() const;
    void setVerticalScale(float scale);
    float verticalScale() const;

private:
    void updateContour();
    void updateGrid();
    void makeTriangleGrid();
    void makeQuadGrid();
    void makeContourFromTriangles();
    void makeContourFromQuads();

private:
    friend class SurfaceProcessor;
    std::shared_ptr <Contour> m_contour;
    std::shared_ptr <SurfaceGrid> m_grid;
    SurfaceProcessorTask m_processingTask;
};

#endif // SURFACE_H
