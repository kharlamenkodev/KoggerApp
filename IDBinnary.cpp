#include "IDBinnary.h"
#include "math.h"

#include <core.h>
extern Core core;

#if defined(Q_OS_ANDROID) || (defined Q_OS_LINUX)
template < typename T, size_t N >
size_t _countof( T const (&array)[ N ] ) { return N; }
#endif

IDBin::IDBin(QObject *parent) : QObject(parent) {
//    setProto(proto);
}

IDBin::~IDBin() {

}

Resp  IDBin::parse(FrameParser &proto) {
    Resp resp_parse = respNone;

    if(proto.type() == CONTENT || proto.type() == SETTING || proto.type() == GETTING) {
        if(proto.resp()) {
           m_lastResp = (Resp)proto.read<U1>();
           resp_parse = respOk;
        } else {
            m_lastResp = respNone;
            resp_parse = parsePayload(proto);
        }

        if(resp_parse == respOk) {
            m_lastType = proto.type();
            m_lastVersion = proto.ver();
            _lastAddress = proto.route();
            emit updateContent(m_lastType, m_lastVersion, m_lastResp, _lastAddress);
        } else {
        }
    }

    return resp_parse;
}

void IDBin::simpleRequest(Version ver) {
    ProtoBinOut req_out;
    req_out.create(GETTING, ver, id(), m_address);
    requestSpecific(req_out);
    req_out.end();

    emit binFrameOut(req_out);
}

void IDBin::appendKey(ProtoBinOut &proto_out) {
    proto_out.write<U4>(m_key);
}

//void IDBin::sendDataProcessing(ProtoBinOut &proto_out) {
//    QByteArray data((char*)proto_out.frame(), proto_out.frameLen());
//    dataSend(data);
//    if(isConsoleOut) { core.consoleProto(proto_out); }
//}


Resp IDBinTimestamp::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {  m_timestamp = proto.read<U4>();
    } else {  return respErrorVersion;  }
    return respOk;
}

Resp IDBinDist::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) { m_dist_mm = proto.read<U4>();
    } else {  return respErrorVersion; }
    return respOk;
}

Resp IDBinChart::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        U2 m_seqOffset = proto.read<U2>();
        U2 sampleResol = proto.read<U2>();
        U2 absOffset = proto.read<U2>();

        if(m_seqOffset == 0 && m_chartSizeIncr != 0) {
//            for(uint16_t i = 0; i < m_chartSizeIncr; i++) {
//                m_completeChart[i] = m_fillChart[i];
//            }
            memcpy(m_completeChart, m_fillChart, m_chartSizeIncr);
            m_chartSize = m_chartSizeIncr;
            m_isCompleteChart = true;
        }

        if(m_seqOffset == 0 || m_sampleResol != sampleResol || m_absOffset != absOffset) {
            m_sampleResol = sampleResol;
            m_absOffset = absOffset;

            m_chartSizeIncr = 0;
        }

        if(m_chartSizeIncr == m_seqOffset) {
            uint16_t part_len = proto.readAvailable();

            if(m_seqOffset + part_len < sizeof (m_fillChart)) {
                proto.read(&m_fillChart[m_chartSizeIncr], part_len);
                m_chartSizeIncr += part_len;
            }
        } else {
            return respErrorPayload;
        }
    } else if(proto.ver() == v6) {
        uint8_t cell_byte_size = proto.read<U1>();
        type = proto.read<U1>();
        channel = proto.read<U2>();
        _lastSeqPosition = proto.read<U4>();

        if(_lastSeqPosition == 0 && _rawSeqPosition != 0) {
            _rawCellCount = _rawSeqPosition;
            _rawDataSize = _rawCellCount*cell_byte_size;
            memcpy(_rawDataSave, _rawData, _rawDataSize);
            m_isCompleteChart = true;
//            qInfo("size %i", m_chartSize);
        }

        if(_lastSeqPosition == 0) {
            _rawSeqPosition = 0;
        }

        if(_rawSeqPosition == _lastSeqPosition) {
            uint16_t part_byte_len = proto.readAvailable();
            uint32_t byte_offset = _rawSeqPosition*cell_byte_size;

            if(byte_offset + part_byte_len < sizeof (_rawData)) {
                proto.read((uint8_t*)&_rawData[byte_offset], part_byte_len);
                _rawSeqPosition += part_byte_len/cell_byte_size;
            }
        } else {
            return respErrorPayload;
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}


Resp IDBinAttitude::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        const float scale_to_deg = 0.01f;
        m_yaw = static_cast<float>(proto.read<S2>())*scale_to_deg;
        m_pitch = static_cast<float>(proto.read<S2>())*scale_to_deg;
        m_roll = static_cast<float>(proto.read<S2>())*scale_to_deg;
    } else if(proto.ver() == v1) {
       m_w0 = proto.read<F4>();
       m_w0 = proto.read<F4>();
       m_w0 = proto.read<F4>();
       m_w0 = proto.read<F4>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

float IDBinAttitude::yaw(Version src_ver) {
    return m_yaw;
}

float IDBinAttitude::pitch(Version src_ver) {
    return m_pitch;
}

float IDBinAttitude::roll(Version src_ver) {
    return m_roll;
}

float IDBinAttitude::w0(Version src_ver) {
    return m_w0;
}
float IDBinAttitude::w1(Version src_ver) {
    return m_w1;
}
float IDBinAttitude::w2(Version src_ver) {
    return m_w2;
}
float IDBinAttitude::w3(Version src_ver) {
    return m_w3;
}

Resp IDBinTemp::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        const float scale_to_cels = 0.01f;
        m_temp = static_cast<float>(proto.read<S2>())*scale_to_cels;
    } else {
        return respErrorVersion;
    }

    return respOk;
}


Resp IDBinNav::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {

    } else {
        return respErrorVersion;
    }

    return respOk;
}

Resp IDBinDVL::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
         vel_x = proto.read<F4>();
         vel_y = proto.read<F4>();
         vel_z = proto.read<F4>();
         _dist = proto.read<F4>();

//         qInfo("DVL x: %f, y: %f, z: %f, d: %f", vel_x, vel_y, vel_z, _dist);
    } else  if(proto.ver() == v1) {
        _beamCount = 0;
        for(uint8_t i = 0; i < 4 && sizeof(BeamSolution) <= proto.readAvailable() ; i++) {
            _beams[i] = proto.read<BeamSolution>();
            _beamCount++;
        }

        if(_beams[0].mode > 0 && _beams[0].mode <= 3) {
            test_bias += _beams[0].velocity*_beams[0].dt;
        }
    } else if(proto.ver() == v2) {
        if(sizeof(DVLSolution) <= proto.readAvailable()) {
            _dvlSolution = proto.read<DVLSolution>();
//            qInfo("DVL x: %f, y: %f, z: %f, d: %f", _dvlSolution.velocity.x, _dvlSolution.velocity.y, _dvlSolution.velocity.z, _dvlSolution.distance.z);
        } else {
            return respErrorPayload;
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}


Resp IDBinDataset::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        uint8_t ch_id = proto.read<U1>();
        if(ch_id < _countof(m_channel)) {
            m_channel[ch_id].id = ch_id;
            m_channel[ch_id].period = proto.read<U4>();
            m_channel[ch_id].mask = proto.read<U4>();
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinDataset::setChannel(uint8_t ch_id, uint32_t period, uint32_t mask) {
    sendChannel(ch_id, period, mask);
}

uint32_t IDBinDataset::mask(U1 ch_id) {
    uint32_t mask = 0;
    if(ch_id == 0) {
        for(int i = 1; i < 3; i++) {
            mask |= m_channel[i].mask;
        }
    } else if(ch_id < _countof(m_channel)) {
        mask = m_channel[ch_id].mask;
    }
    return mask;
}

void IDBinDataset::setMask(U1 ch_id, uint32_t mask) {
    m_channel[ch_id].id = ch_id;
    m_channel[ch_id].mask = mask;
}

uint32_t IDBinDataset::period(U1 ch_id) {
    if(ch_id < _countof(m_channel)) {
        return m_channel[ch_id].period;
    }
    return 0;
}

void IDBinDataset::setPeriod(U1 ch_id, uint32_t period) {
    sendChannel(ch_id, period, mask(ch_id));
}

void IDBinDataset::sendChannel(U1 ch_id, uint32_t period, uint32_t mask) {
    if(ch_id < _countof(m_channel)) {
        m_channel[ch_id].period = period;

        ProtoBinOut id_out;
        id_out.create(SETTING, v0, id(), m_address);
        id_out.write<U1>(ch_id);
        id_out.write<U4>(period);
        id_out.write<U4>(mask);
        id_out.end();
        emit binFrameOut(id_out);
    }
}


Resp IDBinDistSetup::parsePayload(FrameParser &proto) {
    if(proto.ver() == v1) {
        m_startOffset = proto.read<U4>();
        m_maxDist = proto.read<U4>();
    } else if(proto.ver() == v2) {
        m_confidence = proto.read<U1>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinDistSetup::setRange(uint32_t start_offset, uint32_t max_dist) {
    m_startOffset = start_offset;
    m_maxDist = max_dist;

    ProtoBinOut id_out;
    id_out.create(SETTING, v1, id(), m_address);
    id_out.write<U4>(start_offset);
    id_out.write<U4>(max_dist);
    id_out.end();
    emit binFrameOut(id_out);
}

void IDBinDistSetup::setConfidence(int confidence) {
    m_confidence = confidence;

    ProtoBinOut id_out;
    id_out.create(SETTING, v2, id(), m_address);
    id_out.write<U1>(m_confidence);
    id_out.end();
    emit binFrameOut(id_out);
}


Resp IDBinChartSetup::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_sanpleCount = proto.read<U2>();
        m_sanpleResolution = proto.read<U2>();
        m_sanpleOffset = proto.read<U2>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinChartSetup::setV0(uint16_t count, uint16_t resolution, uint16_t offset) {
    m_sanpleCount = count;
    if(count*resolution > 200000) {
        resolution = (200000/count/10)*10;
    }
    m_sanpleResolution = resolution;
    m_sanpleOffset = offset;

    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U2>(count);
    id_out.write<U2>(resolution);
    id_out.write<U2>(offset);
    id_out.end();
    emit binFrameOut(id_out);
}

Resp IDBinDSPSetup::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_horSmoothFactor = proto.read<U1>();
        qInfo("read smooth %u", m_horSmoothFactor);
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinDSPSetup::setV0(U1 hor_smooth_factor) {
    m_horSmoothFactor = hor_smooth_factor;

    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U1>(horSmoothFactor());
    id_out.end();
    qInfo("write smooth %u", m_horSmoothFactor);
    emit binFrameOut(id_out);
}


Resp IDBinTransc::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_freq = proto.read<U2>();
        m_pulse = proto.read<U1>();
        m_boost = proto.read<U1>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinTransc::setTransc(U2 freq, U1 pulse, U1 boost) {
    m_freq = freq;
    m_pulse = pulse;
    m_boost = boost;

    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U2>(freq);
    id_out.write<U1>(pulse);
    id_out.write<U1>(boost);
    id_out.end();
    emit binFrameOut(id_out);
}


Resp IDBinSoundSpeed::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_soundSpeed = proto.read<U4>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinSoundSpeed::setSoundSpeed(U4 snd_spd) {
    m_soundSpeed = snd_spd;

    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U4>(snd_spd);
    id_out.end();
    emit binFrameOut(id_out);
}

Resp IDBinUART::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        if(checkKeyConfirm(proto.read<U4>())) {
            uint8_t uart_id = proto.read<U1>();

            if(uart_id < _countof(m_uart)) {
                m_uart[uart_id].id = uart_id;
                m_uart[uart_id].baudrate = proto.read<U4>();
            }
        } else {
            return respErrorKey;
        }
    } else if(proto.ver() == v1) {
        if(checkKeyConfirm(proto.read<U4>())) {
            uint8_t uart_id = proto.read<U1>();

            if(uart_id < _countof(m_uart)) {
                m_uart[uart_id].id = uart_id;
                m_uart[uart_id].dev_address = proto.read<U1>();
            }
        } else {
            return respErrorKey;
        }
    } else if(proto.ver() == v2) {
        if(checkKeyConfirm(proto.read<U4>())) {
            devDef_address = proto.read<U1>();
        } else {
            return respErrorKey;
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinUART::setBaudrate(U4 baudrate) {
    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    appendKey(id_out);

    id_out.write<U1>(1);
    qInfo("setBaudrate %u", baudrate);
    m_uart[1].baudrate = baudrate;
    id_out.write<U4>(m_uart[1].baudrate);

    id_out.end();
    emit binFrameOut(id_out);
}

void IDBinUART::setDevAddress(U1 addr) {
    ProtoBinOut id_out;
    id_out.create(SETTING, v1, id(), m_address);
    appendKey(id_out);

    id_out.write<U1>(1);
    m_uart[1].dev_address = addr;
    id_out.write<U1>(addr);

    id_out.end();
    emit binFrameOut(id_out);
}

void IDBinUART::setDevDefAddress(U1 addr) {
    ProtoBinOut id_out;
    id_out.create(SETTING, v2, id(), m_address);
    appendKey(id_out);

    devDef_address = addr;
    id_out.write<U1>(addr);

    id_out.end();
    emit binFrameOut(id_out);
}

Resp IDBinVersion::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_boardVersionMinor = proto.read<U1>();
        m_boardVersion = (BoardVersion)proto.read<U1>();
        proto.read<U2>();
        proto.read<U2>();
        proto.read<U2>();
        proto.read<U4>();
        proto.read<U2>();
        m_serialNumber = proto.read<U4>();
    }  else if(proto.ver() == v1) {
        if(proto.readAvailable() == 12) {
            _uid.resize(12);
            proto.read((uint8_t*)_uid.data(), 12);
        } else {
        }
    } else if(proto.ver() == v2) {
        _bootMode = proto.read<U1>();
        m_boardVersionMinor = proto.read<U1>();
        m_boardVersion = (BoardVersion)proto.read<U1>();
        _bootVersionMinor = proto.read<U1>();
        _bootVersion = proto.read<U1>();
        proto.read<U2>();
        _fwVersionMinor = proto.read<U1>();
        _fwVersion = proto.read<U1>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

Resp IDBinMark::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        m_mark = proto.read<U1>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinMark::setMark() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    appendKey(id_out);
    id_out.end();

    emit binFrameOut(id_out);
}


Resp IDBinFlash::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        if(checkKeyConfirm(proto.read<U4>())) {
        } else {
            return respErrorKey;
        }
    } else if(proto.ver() == v1) {
        if(checkKeyConfirm(proto.read<U4>())) {
        } else {
            return respErrorKey;
        }
    } else if(proto.ver() == v2) {
        if(checkKeyConfirm(proto.read<U4>())) {
        } else {
            return respErrorKey;
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinFlash::flashing() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    appendKey(id_out);
    id_out.end();
    emit binFrameOut(id_out);
}


void IDBinFlash::restore() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v1, id(), m_address);
    appendKey(id_out);
    id_out.end();
    emit binFrameOut(id_out);
}

void IDBinFlash::erase() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v2, id(), m_address);
    appendKey(id_out);
    id_out.end();
    emit binFrameOut(id_out);
}


Resp IDBinBoot::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        if(checkKeyConfirm(proto.read<U4>())) {
        } else {
            return respErrorKey;
        }
    } else if(proto.ver() == v1) {
        if(checkKeyConfirm(proto.read<U4>())) {
        } else {
            return respErrorKey;
        }
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinBoot::reboot() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    appendKey(id_out);
    id_out.end();
    emit binFrameOut(id_out);
}

void IDBinBoot::runFW() {
    ProtoBinOut id_out;
    id_out.create(SETTING, v1, id(), m_address);
    appendKey(id_out);
    id_out.end();
    emit binFrameOut(id_out);
}

Resp IDBinUpdate::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        _progress = proto.read<ID_UPGRADE_V0>();
    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinUpdate::setUpdate(QByteArray fw) {
    _fw = fw;
    _currentFwOffset = 0;
    _currentNumPacket = 1;
}

bool IDBinUpdate::putUpdate(bool is_auto_offset) {
    uint16_t len_part = _packetSize;

    if(len_part > availSend()) {
        len_part = (uint16_t)availSend();
    }

    if(len_part == 0) {
        return false;
    }

    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U2>(_currentNumPacket);

    for(uint16_t i = 0; i < len_part; i++) {
        id_out.write<U1>((U1)_fw[i + _currentFwOffset]);
    }

    if(is_auto_offset) {
        _currentFwOffset += len_part;
        _currentNumPacket++;
    }

    id_out.end();
    emit binFrameOut(id_out);

    return true;
}


Resp IDBinVoltage::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {
        uint8_t ch_id = proto.read<U1>();
        int32_t u_v = proto.read<S4>();
        _v[ch_id] =  float(u_v);
    } else {
        return respErrorVersion;
    }

    return respOk;
}

Resp IDBinDVLMode::parsePayload(FrameParser &proto) {
    if(proto.ver() == v0) {

    } else {
        return respErrorVersion;
    }

    return respOk;
}

void IDBinDVLMode::setModes(bool ismode1, bool ismode2, bool ismode3, float range_mode3) {
    ProtoBinOut id_out;
    id_out.create(SETTING, v0, id(), m_address);
    id_out.write<U1>(1);
    id_out.write<U1>(0);
    id_out.write<U1>(0);
    id_out.write<U1>(0);

    DVLModeSetup mode1, mode2, mode3;
    mode1.id = 1;
    mode2.id = 2;
    mode3.id = 3;
    mode1.selection = ismode1;
    mode2.selection = ismode2;
    mode3.selection = ismode3;
    mode3.stop = range_mode3;
    id_out.write<DVLModeSetup>(mode1);
    id_out.write<DVLModeSetup>(mode2);
    id_out.write<DVLModeSetup>(mode3);

    id_out.end();
    emit binFrameOut(id_out);
}
