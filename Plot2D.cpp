#include "Plot2D.h"
#include <epochevent.h>

Plot2D::Plot2D() {
    _echogram.setVisible(true);
    _attitude.setVisible(true);
    _DVLBeamVelocity.setVisible(true);
    _DVLSolution.setVisible(true);
    _bottomProcessing.setVisible(true);
    _rangeFinder.setVisible(true);
    _grid.setVisible(true);
    _aim.setVisible(true);

    setDataChannel(CHANNEL_FIRST);
//    _cursor.attitude.from = -180;
//    _cursor.attitude.to = 180;

    _cursor.distance.set(0, 20);
//    _cursor.velocity.set(-1, 1);
}

bool Plot2D::getImage(int width, int height, QPainter* painter) {
    _canvas.setSize(width, height, painter);
//    _canvas.clear();
    reindexingCursor();
    reRangeDistance();


//    painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    _echogram.draw(_canvas, _dataset, _cursor);

    _attitude.draw(_canvas, _dataset, _cursor);
    _DVLBeamVelocity.draw(_canvas, _dataset, _cursor);
    _DVLSolution.draw(_canvas, _dataset, _cursor);
    _bottomProcessing.draw(_canvas, _dataset, _cursor);
    _rangeFinder.draw(_canvas, _dataset, _cursor);
    _GNSS.draw(_canvas, _dataset, _cursor);

    painter->setCompositionMode(QPainter::CompositionMode_Exclusion);
    _grid.draw(_canvas, _dataset, _cursor);
    _aim.draw(_canvas, _dataset, _cursor);

    return true;
}

void Plot2D::setTimelinePosition(float position) {
    if(position > 1.0f) { position = 1.0f; }
    if(position < 0) { position = 0; }

    if(_cursor.position != position) {
        _cursor.position = position;
        plotUpdate();
    }
}

void Plot2D::scrollPosition(int columns) {
    float new_position = timelinePosition() + (1.0f/_dataset->size())*columns;
    setTimelinePosition(new_position);
}

void Plot2D::setDataChannel(int channel, int channel2) {
    _cursor.channel1 = channel;
    _cursor.channel2 = channel2;

    float from = NAN, to = NAN;

    if(_dataset != NULL) {
        _dataset->getMaxDistanceRange(&from, &to, channel, channel2);

        if(isfinite(from) && isfinite(to) && (to - from) > 0) {
            _cursor.distance.set(from, to);
        }
    }

    resetCash();

    plotUpdate();
}

void Plot2D::setEchogramLowLevel(float low) {
    _echogram.setLowLevel(low);
    plotUpdate();
}

void Plot2D::setEchogramHightLevel(float high) {
    _echogram.setHightLevel(high);
    plotUpdate();
}

void Plot2D::setEchogramVisible(bool visible) {
    _echogram.setVisible(visible);
    _echogram.resetCash();
    plotUpdate();
}

void Plot2D::setEchogramTheme(int theme_id) {
    _echogram.setThemeId(theme_id);
    plotUpdate();
}

void Plot2D::setBottomTrackVisible(bool visible) {
    _bottomProcessing.setVisible(visible);
    plotUpdate();
}

void Plot2D::setBottomTrackTheme(int theme_id) {

}

void Plot2D::setRangefinderVisible(bool visible) {
    _rangeFinder.setVisible(visible);
    _grid.setRangeFinderVisible(visible);
    plotUpdate();
}

void Plot2D::setRangefinderTheme(int theme_id) {
    _rangeFinder.setTheme(theme_id);
    plotUpdate();
}

void Plot2D::setAttitudeVisible(bool visible) {
    _attitude.setVisible(visible);
    plotUpdate();
}

void Plot2D::setDopplerBeamVisible(bool visible, int beam_filter) {
    _DVLBeamVelocity.setVisible(visible);
    _DVLBeamVelocity.setBeamFilter(beam_filter);
    plotUpdate();
}

void Plot2D::setDopplerInstrumentVisible(bool visible) {
    _DVLSolution.setVisible(visible);
    plotUpdate();
}

void Plot2D::setGNSSVisible(bool visible, int flags) {
    _GNSS.setVisible(visible);
    plotUpdate();
}

void Plot2D::setGridVetricalNumber(int grids) {
    _grid.setVisible(grids > 0);
    _grid.setVetricalNumber(grids);
    plotUpdate();
}

void Plot2D::setVelocityVisible(bool visible) {
    _grid.setVelocityVisible(visible);
    plotUpdate();
}

void Plot2D::setVelocityRange(float velocity) {
    _cursor.velocity.from = -velocity;
    _cursor.velocity.to = velocity;
    plotUpdate();
}

void Plot2D::setDistanceAutoRange(int auto_range_type) {
    _cursor.distance.mode = AutoRangeMode(auto_range_type);
}

void Plot2D::setDistance(float from, float to) {
    _cursor.distance.set(from, to);
}

void Plot2D::zoomDistance(float ratio) {
    _cursor.distance.mode = AutoRangeNone;

    int  delta = ratio;
    if(delta == 0) return;

    float from = _cursor.distance.from;
    float to = _cursor.distance.to;
    float absrange = abs(to - from);

    float zoom = delta < 0 ? -delta*0.01f : delta*0.01f;
    float delta_range = absrange*zoom;
    float new_range = 0;

    if(delta_range < 0.1) {
        delta_range = 0.1;
    } else if(delta_range > 5) {
        delta_range = 5;
    }

    if(delta > 0) {
        new_range = absrange + delta_range;
    } else {
        new_range = absrange - delta_range;
    }

    if(new_range < 1) {
        new_range = 1;
    } else if(new_range > 500) {
        new_range = 500;
    }


    if(_cursor.isChannelDoubled()) {
        _cursor.distance.from = -ceil( new_range/2);
        _cursor.distance.to = ceil( new_range/2);
    } else {
       _cursor.distance.to = ceil(_cursor.distance.from + new_range);
    }

    plotUpdate();
}

void Plot2D::scrollDistance(float ratio)    {
    _cursor.distance.mode = AutoRangeNone;

    float from = _cursor.distance.from;
    float to = _cursor.distance.to;
    float absrange = abs(to - from);

    float delta_offset = ((float)absrange*(float)ratio*0.001f);

    if(from < to) {
        float round_cef = 10.0f;

        float from_n = (round((from + delta_offset)*round_cef)/round_cef);
        float to_n = (round((to + delta_offset)*round_cef)/round_cef);

        if(!_cursor.isChannelDoubled()) {
            if(from_n < 0) {
                to_n -= from_n;
                from_n = 0;
            }
        }

        _cursor.distance.from = from_n;
        _cursor.distance.to = to_n;

    } else if(from > to) {
        _cursor.distance.from = (from - delta_offset);
        _cursor.distance.to = (to - delta_offset);
    }

    plotUpdate();
}

void Plot2D::setMousePosition(int x, int y) {

    const int image_width = _canvas.width();
    const int image_height = _canvas.height();
    const int dataset_from = _cursor.getIndex(0);
    const float distance_from = _cursor.distance.from;
    const float distance_range = _cursor.distance.to - _cursor.distance.from;
    const float image_distance_ratio = distance_range/(float)image_height;

    struct {
        int x = -1, y = -1;
    } _mouse;

    _mouse.x = _cursor.mouseX;
    _mouse.y = _cursor.mouseY;
    _cursor.setMouse(x, y);


    if(x < -1) { x = -1; }
    if(x >= image_width) { x = image_width - 1; }

    if(y < 0) { y = 0; }
    if(y >= image_height) { x = image_height - 1; }

    if(x == -1) {
        _mouse.x = -1;
        plotUpdate();
        return;
    }

    int x_start = 0, y_start = 0;
    int x_length = 0;
    float y_scale = 0.0f;
    if(_mouse.x != -1) {
        if(_mouse.x < x) {
            x_length = x - _mouse.x;
            x_start = _mouse.x;
            y_start = _mouse.y;
            y_scale = (float)(y - _mouse.y)/(float)x_length;
        } else if(_mouse.x > x) {
            x_length = _mouse.x - x;
            x_start = x;
            y_start = y;
            y_scale = -(float)(y - _mouse.y)/(float)x_length;
        } else {
            x_length = 1;
            x_start = x;
            y_start = y;
            y_scale = 0;
        }
    } else {
        x_length = 1;
        x_start = x;
        y_start = y;
        y_scale = 0;
    }

//    _mouse.x = x;
//    _mouse.y = y;

    if(_cursor.tool() > MouseToolNothing) {
        for(int x_ind = 0; x_ind < x_length; x_ind++) {
            int epoch_index = _cursor.getIndex(x_start + x_ind);

            Epoch* epoch = _dataset->fromIndex(epoch_index);

            const int channel1 = _cursor.channel1;
            const int channel2 = _cursor.channel2;

            if(epoch != NULL) {

                float image_y_pos = ((float)y_start + (float)x_ind*y_scale);
                float dist = abs(image_y_pos*image_distance_ratio + distance_from);

                if(_cursor.tool() == MouseToolDistanceMin) {
                    epoch->setMinDistProc(channel1, dist);
                    epoch->setMinDistProc(channel2, dist);
                } else if(_cursor.tool() == MouseToolDistance) {
                    epoch->setDistProcessing(channel1, dist);
                    epoch->setDistProcessing(channel2, dist);
                } else if(_cursor.tool()== MouseToolDistanceMax) {
                    epoch->setMaxDistProc(channel1, dist);
                    epoch->setMaxDistProc(channel2, dist);
                } else if(_cursor.tool() == MouseToolDistanceErase) {
                    epoch->clearDistProcessing(channel1);
                    epoch->clearDistProcessing(channel2);
                }
            }
        }

        if(_cursor.tool() == MouseToolDistanceMin || _cursor.tool() == MouseToolDistanceMax) {
            _bottomTrackParam.indexFrom = _cursor.getIndex(x_start);
            _bottomTrackParam.indexTo = _cursor.getIndex(x_start + x_length);
            _dataset->bottomTrackProcessing(_cursor.channel1, _cursor.channel2, _bottomTrackParam);
        }
    }

    plotUpdate();
}

void Plot2D::setMouseTool(MouseTool tool) {
    _cursor.setTool(tool);
}

void Plot2D::resetCash() {
    _echogram.resetCash();
}

void Plot2D::reindexingCursor() {
    if(_dataset == nullptr) { return; }

    const int image_width = _canvas.width();
    const int data_width = _dataset->size();
    const int last_indexes_size = _cursor.indexes.size();

    if(image_width != last_indexes_size) {
        _cursor.indexes.resize(image_width);
    }

    if(_cursor.last_dataset_size > 0) {
        float position = timelinePosition();

        float last_head = round(position*_cursor.last_dataset_size);
        float last_offset_head = float(_cursor.last_dataset_size) - last_head;
        float new_head = data_width - last_offset_head;

        position = float(new_head)/float(data_width);

        setTimelinePosition(position);
    }
    _cursor.last_dataset_size = data_width;

    float hor_ratio = 1.0f;

    float position = timelinePosition();

    int head_data_index = round(position*float(data_width));

    for(int i = 0; i < image_width; i++) {
        int data_index = head_data_index + round((i - image_width)/hor_ratio) - 1;
        if(data_index >= 0 && data_index < data_width) {
             _cursor.indexes[i] = data_index;
        } else {
             _cursor.indexes[i] = -1;
        }
    }
}

void Plot2D::reRangeDistance() {
    if(_dataset == NULL) { return; }
    float max_range = NAN;

    if(_cursor.distance.mode == AutoRangeLastData) {
        for(int i = _dataset->endIndex() - 3; i < _dataset->endIndex(); i++) {
            Epoch* epoch = _dataset->fromIndex(i);
            if(epoch != NULL) {
                float epoch_range = epoch->getMaxRnage(_cursor.channel1);
                if(!isfinite(max_range) || max_range < epoch_range) {
                    max_range = epoch_range;
                }
            }
        }
    }

    if(_cursor.distance.mode == AutoRangeLastOnScreen) {
        for(unsigned int i = _cursor.indexes.size() - 3; i < _cursor.indexes.size(); i++) {
            Epoch* epoch = _dataset->fromIndex(_cursor.getIndex(i));
            if(epoch != NULL) {
                float epoch_range = epoch->getMaxRnage(_cursor.channel1);
                if(!isfinite(max_range) || max_range < epoch_range) {
                    max_range = epoch_range;
                }
            }
        }
    }

    if(_cursor.distance.mode == AutoRangeMaxOnScreen) {
        for(unsigned int i = 0; i < _cursor.indexes.size(); i++) {
            Epoch* epoch = _dataset->fromIndex(_cursor.getIndex(i));
            if(epoch != NULL) {
                float epoch_range = epoch->getMaxRnage(_cursor.channel1);
                if(!isfinite(max_range) || max_range < epoch_range) {
                    max_range = epoch_range;
                }
            }
        }
    }

    if(isfinite(max_range)) {
        if(_cursor.isChannelDoubled()) {
            _cursor.distance.from = -ceil(max_range);;
        } else {
            _cursor.distance.from = 0;
        }
        _cursor.distance.to = ceil(max_range);
    }
}
