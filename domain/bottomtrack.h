#ifndef BOTTOMTRACK_H
#define BOTTOMTRACK_H

#include <sceneobject.h>
#include <plotcash.h>

#include <memory>

class GraphicsScene3dView;
class Surface;
class BottomTrack : public SceneObject
{
    Q_OBJECT
    Q_PROPERTY(DatasetChannel visibleChannel READ visibleChannel WRITE setVisibleChannel NOTIFY visibleChannelChanged FINAL)
    QML_NAMED_ELEMENT(BottomTrack)

public:
    class BottomTrackRenderImplementation : public SceneObject::RenderImplementation
    {
    public:
        BottomTrackRenderImplementation();
        virtual ~BottomTrackRenderImplementation();
        virtual void render(QOpenGLFunctions* ctx,
                            const QMatrix4x4& mvp,
                            const QMap <QString, std::shared_ptr <QOpenGLShaderProgram>>& shaderProgramMap) const override;

        virtual void render(QOpenGLFunctions* ctx,
                            const QMatrix4x4& model,
                            const QMatrix4x4& view,
                            const QMatrix4x4& projection,
                            const QMap <QString, std::shared_ptr <QOpenGLShaderProgram>>& shaderProgramMap) const override;
    private:
        friend class BottomTrack;
        QVector<int> m_selectedVertexIndices;
        bool m_isDisplayingWithSurface = true;
    };

    explicit BottomTrack(GraphicsScene3dView* view = nullptr, QObject* parent = nullptr);
    virtual ~BottomTrack();
    virtual SceneObjectType type() const override;
    virtual bool eventFilter(QObject *watched, QEvent *event);
    QList<Epoch*> epochs() const;
    QMap<int,DatasetChannel> channels() const;
    DatasetChannel visibleChannel() const;

public Q_SLOTS:
    virtual void setData(const QVector<QVector3D>& data, int primitiveType = GL_POINTS) override;
    virtual void clearData() override;
    void setEpochs(const QList<Epoch*>& epochList,const QMap<int,DatasetChannel>& channels);
    void resetVertexSelection();
    void setDisplayingWithSurface(bool displaying);
    void setVisibleChannel(int channelIndex);
    void setVisibleChannel(const DatasetChannel& channel);
    void selectEpoch(int epochIndex, int channelId);

Q_SIGNALS:
    void epochHovered(int epochIndex);
    void epochPressed(int epochIndex);
    void epochErased(int epochIndex);
    void epochSelected(int epochIndex, int channelId);
    void epochListChanged();
    void visibleChannelChanged(int channelId);
    void visibleChannelChanged(DatasetChannel channel);

protected:
    friend class GraphicsScene3dView;

    virtual void mouseMoveEvent(Qt::MouseButtons buttons, qreal x, qreal y) override;
    virtual void mousePressEvent(Qt::MouseButtons buttons, qreal x, qreal y) override;
    virtual void mouseReleaseEvent(Qt::MouseButtons buttons, qreal x, qreal y) override;
    virtual void keyPressEvent(Qt::Key key) override;
    void updateRenderData();

private:
    using EpochIndex = int;
    using VerticeIndex = int;

    QList<Epoch*> m_epochList;
    QMap<int, DatasetChannel> m_channels;
    QHash<VerticeIndex,EpochIndex> m_epochIndexMatchingMap;
    LLARef m_llaRef;
    DatasetChannel m_visibleChannel;
};

#endif // BOTTOMTRACK_H
