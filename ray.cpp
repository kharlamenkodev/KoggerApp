#include "ray.h"
#include <sceneobject.h>

Ray::Ray(QObject *parent)
    : QObject{parent}
{}

Ray::Ray(const QVector3D &origin, const QVector3D &direction, QObject *parent)
    : QObject(parent)
    ,m_origin(origin)
    ,m_direction(direction)
{}

QVector3D Ray::origin() const
{
    return m_origin;
}

QVector3D Ray::direction() const
{
    return m_direction;
}

QVector<RayHit> Ray::hitObject(std::weak_ptr<SceneObject> object, HittingMode hittingMode)
{
    switch(hittingMode){
    case HittingMode::Vertex:
        return pickAsVertex(object);
    case HittingMode::Triangle:
        return pickAsTriangles(object);
    case HittingMode::Quad:
        return pickAsQuads(object);
    case HittingMode::Segment:
        return pickAsSegments(object);
    }

    return {};
}

QVector<RayHit> Ray::pickAsVertex(std::weak_ptr<SceneObject> object)
{
    auto _object = object.lock();
    auto size = _object->cdata().size();

    if(size <= 0) return {};

    RayHit hit;
    hit.setObject(object);

    auto d = _object->cdata().first().distanceToLine(m_origin, m_direction);

    for (int i = 0; i < size; i++){
        auto p = _object->cdata().at(i);
        auto _d = p.distanceToLine(m_origin, m_direction);
        if(_d < d){
            d = _d;
            hit.setIndices(i,i);
            hit.setWorldIntersection(p);
        }
    }

    if(hit.indices().first >= 0)
        return {hit};

    return {};
}

QVector<RayHit> Ray::pickAsTriangles(std::weak_ptr<SceneObject> object)
{
    Q_UNUSED(object)
    return {};
}

QVector<RayHit> Ray::pickAsQuads(std::weak_ptr<SceneObject> object)
{
    Q_UNUSED(object)
    return {};
}

QVector<RayHit> Ray::pickAsSegments(std::weak_ptr<SceneObject> object)
{
    Q_UNUSED(object)
    return {};
}

void Ray::setOrigin(const QVector3D &origin)
{
    if(m_origin != origin)
        m_origin = origin;
}

void Ray::setDirection(const QVector3D &dir)
{
    if(m_direction != dir)
        m_direction = dir;
}

//----------------------->Ray hit<--------------------------//
std::weak_ptr<SceneObject> RayHit::object() const
{
    return m_object;
}

QVector3D RayHit::worldIntersection() const
{
    return m_worldIntersection;
}

QPair<int, int> RayHit::indices() const
{
    return m_indices;
}

void RayHit::setObject(std::weak_ptr<SceneObject> object)
{
    if(m_object.lock() != object.lock())
        m_object = object;
}

void RayHit::setWorldIntersection(const QVector3D &intersection)
{
    if(m_worldIntersection != intersection)
        m_worldIntersection = intersection;
}

void RayHit::setIndices(int begin, int end)
{
    m_indices = {begin,end};
}
