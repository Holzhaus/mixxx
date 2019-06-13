/**
* @file midiclockgenerator.h
* @author Jan Holthuis holthuis.jan@googlemail.com
* @date Thu 13 Jun 2019
* @brief MIDI Clock Generator
*
* This is a base class representing a MIDI controller.
*   It must be inherited by a class that implements it on some API.
*
*   Note that the subclass' destructor should call close() at a minimum.
*/

#ifndef MIDICLOCKGENERATOR_H
#define MIDICLOCKGENERATOR_H

#include <QThread>
#include <QObject>

class MidiController;

class MidiClockGenerator : public QThread
{
    Q_OBJECT

public:
    const int PPQN = 24;
    MidiClockGenerator(MidiController *controller, QObject *parent = nullptr);

public slots:
    void setBPM(double);
    void stop();

private:
    void run() override;

    MidiController *m_controller = NULL;
    bool m_running;
    qint64 m_clockInterval;
    double m_clockIntervalTruncationError;
};

#endif
