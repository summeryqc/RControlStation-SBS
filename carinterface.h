/*
    Copyright 2016-2017 Benjamin Vedder	benjamin@vedder.se

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef CARINTERFACE_H
#define CARINTERFACE_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QUdpSocket>
#include "datatypes.h"
#include "mapwidget.h"
#include "packetinterface.h"
#include "tcpserversimple.h"

#ifdef HAS_OPENGL
#include "orientationwidget.h"
#endif

#ifdef HAS_SBS
#include "nmeawidget.h"
#endif

namespace Ui {
class CarInterface;
}

class CarInterface : public QWidget
{
    Q_OBJECT

public:
    explicit CarInterface(QWidget *parent = 0);
    ~CarInterface();
    void setID(int id);
    int getId();
    bool pollData();
    bool updateRouteFromMap();
    void setOrientation(double roll, double pitch, double yaw);
    void setStateData(CAR_STATE data);
    void setMap(MapWidget *map);
    void setPacketInterface(PacketInterface *packetInterface);
    void setControlValues(double throttle, double steering, double max, bool currentMode);
    void emergencyStop();
    void setCtrlAp();
    void setCtrlKb();
    bool setAp(bool on);

#ifdef HAS_SBS
    NmeaWidget* getNmeaWidget();
    double getSpeed();
    void setUpdateRouteFromMap(bool enabled);
    void setRPiClock();    
#endif 

signals:
    void terminalCmd(quint8 id, QString cmd);
    void forwardVesc(quint8 id, QByteArray data);
    void setRcCurrent(quint8 id, double current, double steering);
    void setRcDuty(quint8 id, double duty, double steering);
    void showStatusInfo(QString str, bool isGood);
    void setServoDirect(quint8 id, double value);

private slots:
    void timerSlot();
    void udpReadReady();
    void tcpRx(QByteArray &data);
    void terminalPrint(quint8 id, QString str);
    void vescFwdReceived(quint8 id, QByteArray data);
    void routePointSet(LocPoint pos);
    void lastRoutePointRemoved();
    void nmeaReceived(quint8 id, QByteArray nmea_msg);
    void configurationReceived(quint8 id, MAIN_CONFIG config);
    void plotInitReceived(quint8 id, QString xLabel, QString yLabel);
    void plotDataReceived(quint8 id, double x, double y);
    void radarSetupReceived(quint8 id, radar_settings_t s);
    void radarSamplesReceived(quint8 id, QVector<QPair<double, double> > samples);
    void dwSampleReceived(quint8 id, DW_LOG_INFO dw);
    void updateAnchorsMap();
    void loadMagCal();

    void on_terminalSendButton_clicked();
    void on_terminalSendVescButton_clicked();
    void on_terminalSendRadarButton_clicked();
    void on_terminalClearButton_clicked();
    void on_idBox_valueChanged(int arg1);
    void on_bldcToolUdpBox_toggled(bool checked);
    void on_vescToolTcpBox_toggled(bool checked);
    void on_autopilotBox_toggled(bool checked);
    void on_clearRouteButton_clicked();
    void on_servoDirectSlider_valueChanged(int value);
    void on_servoMappedSlider_valueChanged(int value);
    void on_confReadButton_clicked();
    void on_confReadDefaultButton_clicked();
    void on_confWriteButton_clicked();
    void on_radarReadButton_clicked();
    void on_radarWriteButton_clicked();
    void on_radarGetRadCCButton_clicked();
    void on_setClockButton_clicked();
    void on_setClockPiButton_clicked();
    void on_rebootPiButton_clicked();
    void on_shutdownPiButton_clicked();
    void on_dwAnch0GetButton_clicked();
    void on_dwAnch1GetButton_clicked();
    void on_dwAnch2GetButton_clicked();
    void on_dwClearSamplesButton_clicked();

#ifdef HAS_SBS
    void on_checkBox_toggled(bool checked);
    void on_pollBox_toggled(bool checked);
#endif

private:
    Ui::CarInterface *ui;
    QVector<double> experimentDataX;
    QVector<double> experimentDataY;
    MapWidget *mMap;
    PacketInterface *mPacketInterface;

#ifdef HAS_OPENGL
    OrientationWidget *mOrientationWidget;
#endif

    int mId;
    CAR_STATE mLastCarState;
    QList<DW_LOG_INFO> mDwData;
    QTimer *mTimer;
    QUdpSocket *mUdpSocket;
    QHostAddress mLastHostAddress;
    quint16 mUdpPort;
    TcpServerSimple *mTcpServer;
    bool mExperimentReplot;

    void getConfGui(MAIN_CONFIG &conf);
    void setConfGui(MAIN_CONFIG &conf);
    void plotDwData();

#ifdef HAS_SBS
    double mSpeed;
#endif

};

#endif // CARINTERFACE_H
