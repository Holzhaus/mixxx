#include <cmath>
#include <QApplication>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include "controllers/midi/midiclockgenerator.h"
#include "controllers/midi/midicontroller.h"

MidiClockGenerator::MidiClockGenerator(MidiController *controller, QObject *parent) : QThread(parent)
{
    m_controller = controller;
}

void MidiClockGenerator::setBPM(double bpm)
{
    m_clockInterval = static_cast<qint64>(std::trunc(Q_INT64_C(60 * 1000 * 1000 * 1000) / bpm / PPQN));
    m_clockIntervalTruncationError = (Q_INT64_C(60 * 1000 * 1000 * 1000) / bpm / PPQN) - m_clockInterval;
    qDebug() << "BPM:" << bpm << m_clockInterval;
}

void MidiClockGenerator::run()
{
    QElapsedTimer timer;
    qint64 currentTime;
    qint64 nextTime;

    double clockDrift = 0.0;
    m_running = true;

    while (m_running)
    {
        timer.start();
        // Send the MidiClock (0xF8) message
        m_controller->sendClockMsg();

        // Calculate next time that we want to send the MidiClock message
        nextTime = m_clockInterval;
        if (clockDrift >= 1.0)
        {
            // If we accumulated a whole nanosecond of clock drift, take care
            // of it
            nextTime += static_cast<qint64>(std::trunc(clockDrift));
            clockDrift -= static_cast<qint64>(std::trunc(clockDrift));
        }

        // Busy wait until we reach the time for the next MidiClock message
        do {
            currentTime = timer.nsecsElapsed();
        }
        while (currentTime < nextTime);

        // Accumuate clock drift
        clockDrift += m_clockIntervalTruncationError;
    }
}

void MidiClockGenerator::stop()
{
    m_running = false;
}
