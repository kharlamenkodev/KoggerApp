#ifndef SCENEOBJECTSLISTMODEL_H
#define SCENEOBJECTSLISTMODEL_H

#include <QAbstractListModel>
#include <QUuid>
#include <QMap>

#include <memory>

#include <vertexobject.h>

class SceneObjectsListModel : public QAbstractListModel
{

    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum RoleNames{
        NameRole = Qt::UserRole,
        IdRole   = Qt::UserRole + 2,
        TypeRole = Qt::UserRole + 3
    };

    explicit SceneObjectsListModel(QObject *parent = nullptr);

    Q_INVOKABLE void insert(int index, const std::shared_ptr <VertexObject> object);

    Q_INVOKABLE void append(const std::shared_ptr <VertexObject> object);

    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE void clear();

    Q_INVOKABLE QStringList names(QString filter = "") const;

    Q_INVOKABLE int objectIndex(QString id) const;

public:

    virtual int rowCount(const QModelIndex& parent) const override;

    virtual QVariant data(const QModelIndex& index, int role) const override;

    QList <std::shared_ptr <VertexObject>> data() const;

    std::shared_ptr <VertexObject> get(QString id) const;

    std::shared_ptr <VertexObject> get(int index) const;

    void replace(int index, std::shared_ptr <VertexObject> object);

    QList <std::shared_ptr <VertexObject>> dataByType(QString type) const;




    int count() const;

protected:

    virtual QHash <int, QByteArray> roleNames() const override;

signals:

    void countChanged(int count);

private:

    QList <std::shared_ptr <VertexObject>> mData;
    QHash <int, QByteArray> mRoleNames;
};

#endif // SCENEOBJECTSLISTMODEL_H