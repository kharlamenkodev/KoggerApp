#ifndef EPOCHEVENT_H
#define EPOCHEVENT_H

#include <QEvent>
#include <plotcash.h>

static const QEvent::Type EpochSelected3d = static_cast<QEvent::Type>(QEvent::User+1);
static const QEvent::Type EpochSelected2d = static_cast<QEvent::Type>(QEvent::User+2);

class Epoch;
class EpochEvent : public QEvent
{
public:
    EpochEvent(QEvent::Type eventType,
               Epoch* epoch,
               int epochIndex,
               const DatasetChannel channel);

    Epoch* epoch() const;
    int epochIndex() const;
    DatasetChannel channel() const;
    bool isValid() const;

private:
    Epoch* m_epoch = nullptr;
    int m_epochIndex = -1;
    int m_channelId = -1;
    DatasetChannel m_channel;
};

#endif // EPOCHEVENT_H
