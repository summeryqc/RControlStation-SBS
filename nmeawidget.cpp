/*
    Copyright 2017 Benjamin Vedder	benjamin@vedder.se

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

#include "nmeawidget.h"
#include "ui_nmeawidget.h"
#include "nmeaserver.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

NmeaWidget::NmeaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NmeaWidget)
{
    ui->setupUi(this);
    layout()->setContentsMargins(0, 0, 0, 0);

    mNmeaForwardServer = new TcpBroadcast(this);

#ifdef HAS_SBS
    ui->nmeaFixTypeLabel->setText("Solution:");
#endif
}

NmeaWidget::~NmeaWidget()
{
    delete ui;
}

void NmeaWidget::inputNmea(QByteArray msg)
{
    if (ui->nmeaPrintBox->isChecked()) {
        ui->nmeaBrowser->append(QString::fromLocal8Bit(msg));
    }

    mNmeaForwardServer->broadcastData(msg);

    NmeaServer::nmea_gga_info_t gga;
    QTextStream msgs(msg);

    while(!msgs.atEnd()) {
        QString str = msgs.readLine();
        QByteArray data = str.toLocal8Bit();

        // Hack
        if (str == "$GPGSA,A,1,,,,,,,,,,,,,,,*1E") {
            ui->nmeaFixTypeLabel->setText("Solution: Invalid");
            ui->nmeaSatsLabel->setText("Satellites: 0");
        }

        if (NmeaServer::decodeNmeaGGA(data, gga) >= 0) {
            QString satStr;
            satStr.sprintf("Satellites: %d", gga.n_sat);
            ui->nmeaSatsLabel->setText(satStr);

            QString fix_type;
            switch (gga.fix_type) {
            case 0: fix_type = "Solution: Invalid"; break;
            case 1: fix_type = "Solution: SPP"; break;
            case 2: fix_type = "Solution: DGPS"; break;
            case 3: fix_type = "Solution: PPS"; break;
            case 4: fix_type = "Solution: RTK Fix"; break;
            case 5: fix_type = "Solution: RTK Float"; break;
            default: fix_type = "Solution: Unknown"; break;
            }

            ui->nmeaFixTypeLabel->setText(fix_type);
            ui->nmeaCorrAgeLabel->setText(QString("Corr age: %1 s").arg(gga.diff_age));
        }
    }
}

void NmeaWidget::on_nmeaLogChooseButton_clicked()
{
    QString path;
    path = QFileDialog::getSaveFileName(this, tr("Choose where to save the NMEA log"));
    if (path.isNull()) {
        return;
    }

    ui->nmeaLogEdit->setText(path);
}

void NmeaWidget::on_nmeaLogActiveBox_toggled(bool checked)
{
    if (checked) {
        bool ok = mNmeaForwardServer->logToFile(ui->nmeaLogEdit->text());

        if (!ok) {
            QMessageBox::warning(this, "NMEA Log",
                                 "Could not open log file.");
            ui->nmeaLogActiveBox->setChecked(false);
        }
    } else {
        mNmeaForwardServer->logStop();
    }
}

void NmeaWidget::on_nmeaServerActiveBox_toggled(bool checked)
{
    if (checked) {
        if (!mNmeaForwardServer->startTcpServer(ui->nmeaServerPortBox->value())) {
            QMessageBox::warning(this, "TCP Server Error",
                                 "Creating TCP server for NMEA data failed. Make sure that the port is not "
                                 "already in use.");
            ui->nmeaServerActiveBox->setChecked(false);
        }
    } else {
        mNmeaForwardServer->stopServer();
    }
}

#ifdef HAS_SBS
void NmeaWidget::on_pushButton_clicked()
{
    ui->nmeaFixTypeLabel->setText("RTK Float");
}

void NmeaWidget::on_pushButton_2_clicked()
{
    ui->nmeaFixTypeLabel->setText("RTK Fix");
}

QString NmeaWidget::getSolution()
{
    return ui->nmeaFixTypeLabel->text();
}

int NmeaWidget::getId()
{
    return mId;
}

void NmeaWidget::setId(int id)
{
    mId = id;
}
#endif








