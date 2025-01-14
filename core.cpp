#include "core.h"
#include <QmlObjectNames.h>
#include <bottomtrack.h>
#include <iomanip>
#include <ctime>
#include <chrono>

Core::Core() : QObject(),
    m_console(new Console()),
    m_connection(new Connection()),
    _dataset(new Dataset)
{
//    m_connection->moveToThread(&connectionThread);
//    connectionThread.start();

    connect(&_devs, &Device::chartComplete, _dataset, &Dataset::addChart);
    connect(&_devs, &Device::iqComplete, _dataset, &Dataset::addIQ);
    connect(&_devs, &Device::distComplete, _dataset, &Dataset::addDist);
    connect(&_devs, &Device::attitudeComplete, _dataset, &Dataset::addAtt);
    connect(&_devs, &Device::positionComplete, _dataset, &Dataset::addPosition);
    connect(&_devs, &Device::dopplerBeamComlete, _dataset, &Dataset::addDopplerBeam);
    connect(&_devs, &Device::dvlSolutionComplete, _dataset, &Dataset::addDVLSolution);
    connect(&_devs, &Device::upgradeProgressChanged, this, &Core::upgradeChanged);

    createControllers();
}

void Core::createControllers()
{
    m_bottomTrackControlMenuController  = std::make_shared<BottomTrackControlMenuController>();
    m_mpcFilterControlMenuController    = std::make_shared<MpcFilterControlMenuController>();
    m_npdFilterControlMenuController    = std::make_shared<NpdFilterControlMenuController>();
    m_surfaceControlMenuController      = std::make_shared<SurfaceControlMenuController>();
    m_pointGroupControlMenuController   = std::make_shared<PointGroupControlMenuController>();
    m_polygonGroupControlMenuController = std::make_shared<PolygonGroupControlMenuController>();
    m_scene3dControlMenuController      = std::make_shared<Scene3DControlMenuController>();
    m_scene3dToolBarController          = std::make_shared<Scene3dToolBarController>();
}

void Core::setEngine(QQmlApplicationEngine *engine)
{
    m_engine = engine;

    QObject::connect(m_engine, &QQmlApplicationEngine::objectCreated,
                     this,      &Core::UILoad, Qt::QueuedConnection);

    m_engine->rootContext()->setContextProperty("BottomTrackControlMenuController",  m_bottomTrackControlMenuController.get());
    m_engine->rootContext()->setContextProperty("SurfaceControlMenuController",      m_surfaceControlMenuController.get());
    m_engine->rootContext()->setContextProperty("PointGroupControlMenuController",   m_pointGroupControlMenuController.get());
    m_engine->rootContext()->setContextProperty("PolygonGroupControlMenuController", m_polygonGroupControlMenuController.get());
    m_engine->rootContext()->setContextProperty("MpcFilterControlMenuController",    m_mpcFilterControlMenuController.get());
    m_engine->rootContext()->setContextProperty("NpdFilterControlMenuController",    m_npdFilterControlMenuController.get());
    m_engine->rootContext()->setContextProperty("Scene3DControlMenuController",      m_scene3dControlMenuController.get());
    m_engine->rootContext()->setContextProperty("Scene3dToolBarController",          m_scene3dToolBarController.get());
}

void Core::consoleProto(FrameParser &parser, bool is_in) {
    QString str_mode;
    QString comment = "";

    switch (parser.type()) {
    case CONTENT:
        str_mode = "DATA";
        if(parser.resp()) {
            switch(parser.frame()[6]) {
            case respNone: comment = "[respNone]"; break;
            case respOk: comment = "[respOk]"; break;
            case respErrorCheck: comment = "[respErrorCheck]"; break;
            case respErrorPayload: comment = "[respErrorPayload]"; break;
            case respErrorID: comment = "[respErrorID]"; break;
            case respErrorVersion: comment = "[respErrorVersion]"; break;
            case respErrorType: comment = "[respErrorType]"; break;
            case respErrorKey: comment = "[respErrorKey]"; break;
            case respErrorRuntime: comment = "[respErrorRuntime]"; break;
            default:
                comment = QString("[resp %1]").arg((int)parser.frame()[6]);
                break;
            }
        } else {
            if(parser.id() == ID_EVENT) {
                comment = QString("Event ID %1").arg(*(uint32_t*)(&parser.frame()[10]));
            }
        }
        break;
    case SETTING:
        str_mode = "SET";
        break;
    case GETTING:
        str_mode = "GET";
        break;
    default:
        str_mode = "NAN";
        break;
    }

    QString str_dir;
    if(is_in) { str_dir = "-->> "; }
    else { str_dir = "<<-- "; }

    try {
        QString str_data = QByteArray((char*)parser.frame(), parser.frameLen()).toHex();

        consoleInfo(QString("%1KG[%2]: id %3 v%4, %5, len %6; %7 [ %8 ]").arg(str_dir).arg(parser.route()).arg(parser.id()).arg(parser.ver()).arg(str_mode).arg(parser.payloadLen()).arg(comment).arg(str_data));

    }catch(std::bad_alloc& ex){
        qCritical().noquote() << __func__ << " --> " << ex.what();
    }
}

QList<QSerialPortInfo> Core::availableSerial(){
    return m_connection->availableSerial();
}

QStringList Core::availableSerialName(){
    consoleInfo("Scaning serial ports...");
    QStringList serialNameList;
    const QList<QSerialPortInfo> serialList = availableSerial();
    consoleInfo((QString("Find serial ports: %1").arg(serialList.size())));
    for (const auto& serial : serialList) {
//        if (!serial.portName().startsWith(QStringLiteral("cu."), Qt::CaseInsensitive)) {
            serialNameList.append(serial.portName());
            consoleInfo("Find serial:" + serial.portName());
//        }
    }
    return serialNameList;
}

bool Core::openConnectionAsSerial(const int id, bool autoconn, const QString &name, int baudrate, bool mode) {
    closeConnection();
    devsConnection();

    m_connection->openSerial(name, baudrate, false);
    m_connection->setRTS(false); // power on

    return true;
}

bool Core::devsConnection() {
    connect(m_connection, &Connection::closedEvent, this, &Core::connectionChanged);
    connect(m_connection, &Connection::openedEvent, this, &Core::connectionChanged);

    connect(m_connection, &Connection::openedEvent, &_devs, &Device::startConnection);
    connect(m_connection, &Connection::receiveData, &_devs, &Device::putData);
    connect(&_devs, &Device::dataSend, m_connection, &Connection::sendData);
    connect(m_connection, &Connection::loggingStream, &_logger, &Logger::loggingStream);

    if(_isLogging) {
        _logger.startNewLog();
    }


    return true;
}

bool Core::openConnectionAsFile(const int id, const QString &name, bool is_append) {
    closeConnection();

    if(!is_append) {
        _dataset->resetDataset();
    }

    QStringList splitname = name.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    if(splitname.size() > 1) {
        QString format = splitname.last();
        if(format.contains("xtf", Qt::CaseInsensitive)) {

            QFile file;
            QUrl url(name);
            if(url.isLocalFile()) {
                file.setFileName(url.toLocalFile());
            } else {
                file.setFileName(url.toString());
            }

            if(file.open(QIODevice::ReadOnly)) {
                return openXTF(file.readAll());
            }

            return false;
        }
    }

    if(m_scene3dView)
        m_scene3dView->clear();

    connect(m_connection, &Connection::openedEvent, &_devs, &Device::startConnection);
    connect(m_connection, &Connection::receiveData, &_devs, &Device::putData);
    m_connection->openFile(name);


    QList<DatasetChannel> chs = _dataset->channelsList().values();


    for(int i = 0; i < _plots2d.size(); i++) {
        if(i == 0 &&_plots2d.at(i) != NULL) {
            if(chs.size() >= 2) {
                _plots2d.at(i)->setDataChannel(chs[0].channel, chs[1].channel);
            }

            if(chs.size() == 1) {
                _plots2d.at(i)->setDataChannel(chs[0].channel);
            }
        }
    }

    return true;

}
bool Core::openConnectionAsIP(const int id, bool autoconn, const QString &address, const int port, bool is_tcp) {
    connect(m_connection, &Connection::closedEvent, this, &Core::connectionChanged);
    connect(m_connection, &Connection::openedEvent, this, &Core::connectionChanged);

    connect(m_connection, &Connection::openedEvent, &_devs, &Device::startConnection);
    connect(m_connection, &Connection::receiveData, &_devs, &Device::putData);
    connect(&_devs, &Device::dataSend, m_connection, &Connection::sendData);
    connect(m_connection, &Connection::loggingStream, &_logger, &Logger::loggingStream);
    m_connection->openIP(address, port, is_tcp);
    return false;
}

bool Core::isOpenConnection() {
    return m_connection->isOpen();
}

bool Core::closeConnection() {
    m_connection->close();
    _devs.stopConnection();

    m_connection->disconnect(&_devs);
    _devs.disconnect(m_connection);

    m_connection->disconnect(this);
    this->disconnect(m_connection);

    m_connection->disconnect(&_logger);

#ifdef FLASHER
    m_connection->disconnect(&flasher);
    flasher.disconnect(m_connection);
    flasher.disconnect(this);
#endif

    _logger.stopLogging();


    return true;
}

bool Core::openProxy(const QString &address, const int port, bool is_tcp) {
    return false;
}

bool Core::closeProxy() {
    return false;
}

bool Core::connectionBaudrate(int baudrate) {
    return m_connection->setBaudrate(baudrate);
}

bool Core::upgradeFW(const QString &name, QObject* dev) {
    QUrl url(name);
    QFile m_file;

    if(url.isLocalFile()) {
        m_file.setFileName(url.toLocalFile());
    } else {
        m_file.setFileName(name);
    }

    bool is_open = false;
    is_open = m_file.open(QIODevice::ReadOnly);

    if(is_open == false) {  return false;  }

    DevQProperty* dev_q = (DevQProperty*)(dev);
    dev_q->sendUpdateFW(m_file.readAll());
//    setUpgradeBaudrate();

    return true;
}

void Core::upgradeChanged(int progress_status) {
    if(progress_status == DevDriver::successUpgrade) {
//        restoreBaudrate();
    }
}

void Core::setLogging(bool is_logging) {
    if(m_connection->isOpen()) {
        if(isLogging() && !is_logging) {
            _logger.stopLogging();
        } else if(!isLogging() && is_logging) {
            _logger.startNewLog();
        }
    }
    _isLogging = is_logging;
}

bool Core::isLogging() {
    return _isLogging;
}

bool Core::exportPlotAsCVS(QString file_path, int channel, float decimation) {
    QString export_file_name;
    if(m_connection->lastType() == Connection::ConnectionFile) {
        export_file_name = m_connection->lastFileName().section('/', -1).section('.', 0, 0);
    } else {
        export_file_name = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh:mm:ss").replace(':', '.');
    }

    _logger.creatExportStream(file_path + "/" + export_file_name + ".csv");

    bool meas_nbr = true;
    bool event_id = true;
    bool rangefinder = false;
    bool bottom_depth = true;
    bool pos_lat_lon = true;
    bool pos_time = true;

    bool external_pos_lla = true;
    bool external_pos_neu = true;
    bool sonar_height = true;
    bool bottom_height = true;

    bool ext_pos_lla_find = false;
    bool ext_pos_ned_find = false;

    int row_cnt = _dataset->size();
    _dataset->spatialProcessing();

    for(int i = 0; i < row_cnt; i++) {
        Epoch* epoch = _dataset->fromIndex(i);

        Position position = epoch->getExternalPosition();
        ext_pos_lla_find |= position.lla.isValid();
        ext_pos_ned_find |= position.ned.isValid();
    }


    if(meas_nbr) {
        _logger.dataExport("Number,");
    }

    if(event_id) {
        _logger.dataExport("Event UNIX,");
        _logger.dataExport("Event timestamp,");
        _logger.dataExport("Event ID,");
    }

    if(rangefinder) {
        _logger.dataExport("Rangefinder,");
    }

    if(bottom_depth) {
        _logger.dataExport("Beam distance,");
    }

    if(pos_lat_lon) {
        _logger.dataExport("Latitude,");
        _logger.dataExport("Longitude,");

        if(pos_time) {
            _logger.dataExport("GNSS UTC Date,");
            _logger.dataExport("GNSS UTC Time,");
        }
    }

    if(external_pos_lla && ext_pos_lla_find) {
        _logger.dataExport("ExtLatitude,");
        _logger.dataExport("ExtLongitude,");
        _logger.dataExport("ExtAltitude,");
    }

    if(external_pos_neu && ext_pos_ned_find) {
        _logger.dataExport("ExtNorth,");
        _logger.dataExport("ExtEast,");
        _logger.dataExport("ExtHeight,");
    }

    if(sonar_height) {
        _logger.dataExport("SonarHeight,");
    }

    if(bottom_height) {
        _logger.dataExport("BottomHeight,");
    }



    _logger.dataExport("\n");



    int prev_timestamp = 0;
    int prev_unix = 0;
    int prev_event_id = 0;
    float prev_dist_proc = 0;
    double prev_lat = 0, prev_lon = 0;

    float decimation_m = decimation;
    float decimation_path = 0;
    LLARef lla_ref;
    NED last_pos_ned;




    for(int i = 0; i < row_cnt; i++) {
        Epoch* epoch = _dataset->fromIndex(i);

        if(decimation_m > 0) {
            if(!epoch->isPosAvail()) { continue; }

            Position pos = epoch->getPositionGNSS();

            if(pos.lla.isCoordinatesValid()) {
                if(!lla_ref.isInit) {
                    lla_ref = LLARef(pos.lla);
                    pos.LLA2NED(&lla_ref);
                    last_pos_ned = pos.ned;
                } else {
                    pos.LLA2NED(&lla_ref);

                    float dif_n = pos.ned.n - last_pos_ned.n;
                    float dif_e = pos.ned.e - last_pos_ned.e;

                    last_pos_ned = pos.ned;

                    decimation_path += sqrtf(dif_n*dif_n + dif_e*dif_e);

                    if(decimation_path < decimation_m) {
                        continue;
                    }

                    decimation_path -= decimation_m;
                }


            } else {
                continue;
            }
        }



        QString row_data;

        if(meas_nbr) {
            row_data.append(QString("%1,").arg(i));
        }

        if(event_id) {
            if(epoch->eventAvail()) {
                prev_timestamp = epoch->eventTimestamp();
                prev_event_id = epoch->eventID();
                prev_unix = epoch->eventUnix();
            }
            row_data.append(QString("%1,%2,%3,").arg(prev_unix).arg(prev_timestamp).arg(prev_event_id));
        }

        if(rangefinder) {
            if(epoch->distAvail()) {
                row_data.append(QString("%1,").arg((float)epoch->rangeFinder()));
            } else {
                row_data.append("0,");
            }
        }

        if(bottom_depth) {
            prev_dist_proc = epoch->distProccesing(channel);
            row_data.append(QString("%1,").arg((float)(prev_dist_proc)));
        }

        if(pos_lat_lon) {
            if(epoch->isPosAvail()) {
                prev_lat = epoch->lat();
                prev_lon = epoch->lon();
            }

            row_data.append(QString::number(prev_lat, 'f', 8));
            row_data.append(",");
            row_data.append(QString::number(prev_lon, 'f', 8));
            row_data.append(",");

            if(pos_time) {
                if(epoch->isPosAvail() && epoch->positionTimeUnix() != 0) {
                    DateTime time_epoch = *epoch->time();

                    DateTime* dt = epoch->time();
                    if(time_epoch.sec > 0) {
                      time_epoch.sec -= 18;
                      dt = &time_epoch;
                    }
//                    DateTime* dt = epoch->positionTime();
                    volatile tm t_sep = dt->getDateTime();
                    t_sep.tm_year += 1900;
                    t_sep.tm_mon += 1;

                    row_data.append(QString("%1-%2-%3").arg(t_sep.tm_year).arg(t_sep.tm_mon).arg(t_sep.tm_mday));
                    row_data.append(",");
                    row_data.append(QString("%1:%2:%3").arg(t_sep.tm_hour).arg(t_sep.tm_min).arg((double)t_sep.tm_sec+(double)dt->nanoSec/1e9));
                    row_data.append(",");
                } else {
                    row_data.append(",");
                    row_data.append(",");
                }
            }
        }

        Position position = epoch->getExternalPosition();

        if(external_pos_lla && ext_pos_lla_find) {
            row_data.append(QString::number(position.lla.latitude, 'f', 10));
            row_data.append(",");
            row_data.append(QString::number(position.lla.longitude, 'f', 10));
            row_data.append(",");
            row_data.append(QString::number(position.lla.altitude, 'f', 3));
            row_data.append(",");
        }

        if(external_pos_neu && ext_pos_ned_find) {
            row_data.append(QString::number(position.ned.n, 'f', 10));
            row_data.append(",");
            row_data.append(QString::number(position.ned.e, 'f', 10));
            row_data.append(",");
            row_data.append(QString::number(-position.ned.d, 'f', 3));
            row_data.append(",");
        }

        Epoch::Echogram* sensor = epoch->chart(channel);

        if(sonar_height) {
            if(sensor != NULL && isfinite(sensor->sensorPosition.ned.d)) {
                row_data.append(QString::number(-sensor->sensorPosition.ned.d, 'f', 3));
            } else  if(sensor != NULL && isfinite(sensor->sensorPosition.lla.altitude)) {
                row_data.append(QString::number(sensor->sensorPosition.lla.altitude, 'f', 3));
            }

            row_data.append(",");
        }

        if(bottom_height) {
            if(sensor != NULL && isfinite(sensor->bottomProcessing.bottomPoint.ned.d)) {
                row_data.append(QString::number(-sensor->bottomProcessing.bottomPoint.ned.d, 'f', 3));
            } else if(sensor != NULL && isfinite(sensor->bottomProcessing.bottomPoint.lla.altitude)) {
                row_data.append(QString::number(sensor->bottomProcessing.bottomPoint.lla.altitude, 'f', 3));
            }
            row_data.append(",");
        }


        row_data.append("\n");
        _logger.dataExport(row_data);
    }

    _logger.endExportStream();

    return true;
}

bool Core::exportPlotAsXTF(QString file_path) {
    QString export_file_name;
    if(m_connection->lastType() == Connection::ConnectionFile) {
        export_file_name = m_connection->lastFileName().section('/', -1).section('.', 0, 0);
    } else {
        export_file_name = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh:mm:ss").replace(':', '.');
    }

    _logger.creatExportStream(file_path + "/_" + export_file_name + ".xtf");


    QMap<int, DatasetChannel> chs = _dataset->channelsList();
    QByteArray data_export = _converterXTF.toXTF(dataset(), _plots2d[0]->plotDatasetChannel(), _plots2d[0]->plotDatasetChannel2());

    _logger.dataByteExport(data_export);

    _logger.endExportStream();
    return true;
}

bool Core::openXTF(QByteArray data) {
    _dataset->resetDataset();

    _converterXTF.toDataset(data, dataset());

    consoleInfo("XTF note:" + QString(_converterXTF.header.NoteString));
    consoleInfo("XTF programm name:" + QString(_converterXTF.header.RecordingProgramName));
    consoleInfo("XTF sonar name:" + QString(_converterXTF.header.SonarName));

    QMap<int, DatasetChannel> chs = _dataset->channelsList();

    for(int i = 0; i < _plots2d.size(); i++) {
        if(_plots2d.at(i) != NULL && i < chs.size()) {
            if(i == 0) {
                _plots2d.at(i)->setDataChannel(chs[0].channel, chs[1].channel);
            }
        }
    }

    return true;
}

bool Core::openCSV(QString name, int separator_type, int first_row, int col_time, bool is_utc_time, int col_lat, int col_lon, int col_altitude, int col_north, int col_east, int col_up) {

    QFile file;
    QUrl url(name);
    if(url.isLocalFile()) {
        file.setFileName(url.toLocalFile());
    } else {
        file.setFileName(url.toString());
    }

    if(!file.open(QIODevice::ReadOnly)) {
        return false;
    }


    QString separator("");
    switch(separator_type) {
        case 0: separator = ","; break;
        case 1: separator = "	"; break;
        case 2: separator = " "; break;
        case 3: separator = ";"; break;
    default: separator = QString((char)separator_type);
    }

    QList<Position> track;

    QTextStream in(&file);
    int skip_rows = first_row - 1;
    while (!in.atEnd()) {
        QString row = in.readLine();
        if(skip_rows > 0) {
            skip_rows--;
            continue;
        }

        if(row[0] == '%' || row[0] == '#') {
            continue;
        }

        QStringList columns = row.split(separator);

        track.append(Position());

//        bool is_glue_date_time = datetime_format.contains(separator) && (col_time < columns.size());




        if(col_time > 0 && (col_time-1 < columns.size()) ) {
//            int y = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0, nsec = 0;
//            sscanf(columns[col_time-1], time_format, &y, &month, &day, &hour, );

            int year = -1, month = -1, day = -1, hour = -1, minute = -1;
            double sec = -1;

            columns[col_time-1].replace(QLatin1Char('/'), QLatin1Char('-'));

            QStringList date_time = columns[col_time-1].split(' ');
            QString date, time;

            if(date_time.size() > 0) {
                if(date_time[0].contains('-')) {
                    date = date_time[0];
                }
            }

            if(date_time.size() == 2) {
                if(date_time[1].contains(':')) {
                    time = date_time[1];
                }
            } else if(date_time.size() == 1) {
                if(col_time < columns.size()) {
                    if(columns[col_time].contains(':')) {
                        time = columns[col_time];
                    }
                }
            }

            QStringList data_sep = date.split('-');
            if(data_sep.size() >= 3) {
                year = data_sep[0].toInt();
                month = data_sep[1].toInt();
                day = data_sep[2].toInt();
            }

            QStringList time_sep = time.split(':');
            if(time_sep.size() >= 3) {
                hour = time_sep[0].toInt();
                minute = time_sep[1].toInt();
                sec = time_sep[2].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
            }

            if(year >= 0 && month >= 0 && day >= 0 && hour >= 0 && minute >= 0 && sec >= 0) {
                int sec_int = (int)sec;
                double nano_sec = (sec - sec_int)*1e9;
                track.last().time = DateTime(year, month, day, hour, minute, sec_int, round(nano_sec));
                if(!is_utc_time) {
                    track.last().time.addSecs(-18);
                }


//                QDateTime time;
//                time.setTimeSpec(Qt::UTC);
//                time.setTime(QTime(hour, minute, sec_int, nano_sec/1e6));
//                time.setDate(QDate(year, month, day));
//                int64_t unix_msec = time.toMSecsSinceEpoch();
//                track.last().time.sec = unix_msec/1000;
//                track.last().time.nanoSec = (unix_msec%1000)*1e6;
            }
        }

        if(col_lat > 0 && col_lat-1 < columns.size()) {
            track.last().lla.latitude = columns[col_lat-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
        }

        if(col_lon > 0 && col_lon-1 < columns.size()) {
            track.last().lla.longitude = columns[col_lon-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
        }

        if(col_altitude > 0 && col_altitude-1 < columns.size()) {
            track.last().lla.altitude = columns[col_altitude-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
        }

        if(col_north > 0 && col_north-1 < columns.size()) {
            track.last().ned.n = columns[col_north-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
        }

        if(col_east > 0 && col_east-1 < columns.size()) {
            track.last().ned.e = columns[col_east-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();
        }

        if(col_up > 0 && col_up-1 < columns.size()) {
            track.last().ned.d = -columns[col_up-1].replace(QLatin1Char(','), QLatin1Char('.')).toDouble();;
        }
    }

    _dataset->mergeGnssTrack(track);

    return true;

}

void Core::restoreBaudrate() {
    m_connection->setBaudrate(backupBaudrate);
}

void Core::setUpgradeBaudrate() {
    backupBaudrate = m_connection->baudrate();
    m_connection->setBaudrate(115200);
}

void Core::UILoad(QObject *object, const QUrl &url) {

    Q_UNUSED(url)

    m_scene3dView = object->findChild<GraphicsScene3dView*> ();

    _plots2d = object->findChildren<qPlot2D*>();

    m_scene3dView->setDataset(_dataset);

    for(int i = 0; i < _plots2d.size(); i++) {
        if(_plots2d.at(i) != NULL) {
            _plots2d.at(i)->setPlot(_dataset);
            m_scene3dView->bottomTrack()->installEventFilter(_plots2d.at(i));
            _plots2d.at(i)->installEventFilter( m_scene3dView->bottomTrack().get());
        }
    }

    //if(m_scene3dView){
    //    QObject::connect(mpBottomTrackProvider.get(), &BottomTrackProvider::bottomTrackChanged,
    //        [this](QVector<QVector3D>& data){
    //            m_scene3dView->bottomTrack()->setData(data, GL_LINE_STRIP);
    //        });
    //}

    m_bottomTrackControlMenuController->setQmlEngine(object);
    m_bottomTrackControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_surfaceControlMenuController->setQmlEngine(object);
    m_surfaceControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_npdFilterControlMenuController->setQmlEngine(object);
    m_npdFilterControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_mpcFilterControlMenuController->setQmlEngine(object);
    m_mpcFilterControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_pointGroupControlMenuController->setQmlEngine(object);
    m_pointGroupControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_polygonGroupControlMenuController->setQmlEngine(object);
    m_polygonGroupControlMenuController->setGraphicsSceneView(m_scene3dView);

    m_scene3dToolBarController->setQmlEngine(object);
    m_scene3dToolBarController->setGraphicsSceneView(m_scene3dView);

    m_scene3dControlMenuController->setQmlEngine(object);
    m_scene3dControlMenuController->setGraphicsSceneView(m_scene3dView);
}

void Core::closing()
{

}
