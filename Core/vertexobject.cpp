#include "vertexobject.h"

VertexObject::VertexObject(const int type)
: mPrimitiveType(type)
{

}

VertexObject::VertexObject(const int type, const QVector<QVector3D> &data)
: mPrimitiveType(type)
, mData(data)
{  
}

VertexObject::~VertexObject()
{

}

void VertexObject::setPrimitiveType(const int type)
{
    mPrimitiveType = type;
}

void VertexObject::setData(const QVector<QVector3D> &data)
{
    mData = data;

    createBounds();
}

void VertexObject::append(const QVector3D &vertex)
{
    mData.append(vertex);

    createBounds();
}

void VertexObject::append(const QVector<QVector3D> &vertexVector)
{
    mData.append(vertexVector);

    createBounds();
}

int VertexObject::primitiveType() const
{
    return mPrimitiveType;
}

QVector<QVector3D> VertexObject::data() const
{
    return mData;
}

const QVector<QVector3D> &VertexObject::cdata() const
{
    return mData;
}

float VertexObject::maximumZ() const
{
    return mMaximumZ;
}

float VertexObject::minimumZ() const
{
    return mMinimumZ;
}

Cube VertexObject::bounds() const
{
    return mBounds;
}

void VertexObject::createBounds()
{
    if (mData.isEmpty()){
        mBounds = Cube(0.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f);
        return;
    }

    float z_max = mData.first().z();
    float z_min = z_max;

    float x_max = mData.first().x();
    float x_min = x_max;

    float y_max = mData.first().y();
    float y_min = y_max;

    for (const auto& v: mData){
        z_min = std::min(z_min, v.z());
        z_max = std::max(z_max, v.z());

        x_min = std::min(x_min, v.x());
        x_max = std::max(x_max, v.x());

        y_min = std::min(y_min, v.y());
        y_max = std::max(y_max, v.y());
    }

    mBounds = Cube(x_min, x_max, y_min, y_max, z_min, z_max);
}

