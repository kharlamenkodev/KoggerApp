#ifndef WATERFALL_H
#define WATERFALL_H

#include <QImage>
#include <QQuickPaintedItem>
#include <QObject>
#include <plotcash.h>
#include <QTimer>
#include "Plot2D.h"

//#define USE_PIXMAP


class qPlot2D : public QQuickPaintedItem, public Plot2D
{
    Q_OBJECT
public:
    Q_PROPERTY(bool horizontal READ isHorizontal() WRITE setHorizontal)
    Q_PROPERTY(float timelinePosition READ timelinePosition WRITE setTimelinePosition NOTIFY timelinePositionChanged)

    qPlot2D(QQuickItem* parent = nullptr);
    void paint(QPainter *painter) override;
//    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

    void setPlot(Dataset* plot);
    bool isHorizontal() { return _isHorizontal; }
    void setHorizontal(bool is_horizontal) { _isHorizontal = is_horizontal;  updater(); }

    void plotUpdate() override;

    virtual bool eventFilter(QObject *watched, QEvent *event);

protected:
    Dataset* m_plot = nullptr;
    QTimer* m_updateTimer;
    bool m_needUpdate = true;
    bool _isHorizontal = true;

signals:
    void timelinePositionChanged();

protected slots:
    void timerUpdater();
    void dataUpdate() { plotUpdate(); }

public slots:
    void updater();
    void horScrollEvent(int delta);
    void verZoomEvent(int delta);
    void verScrollEvent(int delta);
    void plotMousePosition(int x, int y);
    void plotMouseTool(int mode);

    void plotDatasetChannel(int channel, int channel2 = CHANNEL_NONE) { setDataChannel(channel, channel2); }
    int plotDatasetChannel() { return _cursor.channel1; }
    int plotDatasetChannel2() { return _cursor.channel2; }

    void plotEchogramVisible(bool visible) { setEchogramVisible(visible); }
    void plotEchogramTheme(int theme_id) { setEchogramTheme(theme_id); }
    void plotBottomTrackVisible(bool visible) { setBottomTrackVisible(visible); }
    void plotBottomTrackTheme(int theme_id) { setBottomTrackTheme(theme_id); }

    void plotRangefinderVisible(bool visible) { setRangefinderVisible(visible); }
    void plotRangefinderTheme(int theme_id) { setRangefinderTheme(theme_id); }
    void plotAttitudeVisible(bool visible) { setAttitudeVisible(visible); }

    void plotDopplerBeamVisible(bool visible, int beam_filter) { setDopplerBeamVisible(visible, beam_filter); }
    void plotDopplerInstrumentVisible(bool visible) { setDopplerInstrumentVisible(visible); }

    void plotGNSSVisible(bool visible, int flags) { setGNSSVisible(visible, flags);}

    void plotGridVerticalNumber(int grids) { setGridVetricalNumber(grids); }
    void plotVelocityVisible(bool visible) { setVelocityVisible(visible); }
    void plotVelocityRange(float velocity) { setVelocityRange(velocity); }

    void plotDistanceAutoRange(int auto_range_type) { setDistanceAutoRange(auto_range_type); }

    void plotEchogramSetLevels(float low, float hight) {
        setEchogramLowLevel(low);
        setEchogramHightLevel(hight);
    }

    void doDistProcessing(int preset, int window_size, float vertical_gap, float range_min, float range_max, float gain_slope, float threshold, float offsetx, float offsety, float offsetz);

};

#endif // WATERFALL_H
