//###################################################################
//! Kogger
//!
//! @author Харламенко И.В.
//! @date 2023
//###################################################################

#pragma once

#include <vertexobject.h>

#include <QColor>
#include <QVector4D>

/**
 * @brief класс отображаемого на 3D - сцене объекта.
 */
class DisplayedObject : public VertexObject
{
    Q_OBJECT

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(QColor color READ color     WRITE setColor   NOTIFY colorChanged)

public:

    /**
     *  @brief Конструктор с параметрами.
     *  @param[in] type - тип примитива для отображения в движке openGL
     *  (из набора дефайнов gl.h).
     */
    DisplayedObject(const int type, QObject* parent = nullptr);

    /**
     *  @brief Конструктор с параметрами.
     *  @param[in] type - тип примитива для отображения в движке openGL
     *  (из набора дефайнов gl.h).
     *  @param[in] data - ссылка на набор вершин объекта.
     */
    DisplayedObject(const int type, const QVector <QVector3D>& data, QObject* parent = nullptr);


    /**
     *  @brief Деструктор.
     */
    virtual ~DisplayedObject();

    /**
     *  @brief Возвращает признак видимости объекта.
     *  @return Признак видимости объекта.
     */
    bool isVisible() const;

    /**
     *  @brief Устанавливает признак видимости объекта.
     *  @param[in] isVisible Признак видимости.
     */
    void setVisible(bool isVisible);

    /**
     *  @brief Устанавливает цвет объекта.
     *  @param[in] color Цвет объекта.
     */
    void setColor(QColor color);

    /**
     *  @brief Возвращает цвет объекта в виде четырехкомпонентного вектора.
     *  @return Цвет объекта.
     */
    QVector4D color4d() const;

    QColor color();

    float lineWidth() const;

    void setLineWidth(float lineWidth);

signals:
    void visibilityChanged(bool isVisible);

    void colorChanged(QColor color);

protected:

    bool mIsVisible  = true;                                   ///< Признак видимости объекта
    QColor mColor    = QColor(255.0f, 255.0f, 255.0f, 255.0f); ///< Цвет объекта
    float mLineWidth = 1.0f;                                   ///< Толщина линии (если тип примитива GL_LINES)
};
