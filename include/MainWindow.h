#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QList>
#include <QVector>  // Include QVector for signals/slots compatibility
#include <QAudioDeviceInfo>  // Include this for QList<QAudioDeviceInfo>
#include "AudioInput.h"
#include "AudioWorker.h"
#include "FrequencyMeter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDeviceChanged(int index);
    void onStartRecording();
    void onStopRecording();
    void onSaveRecording();
    void updateDisplay(const QVector<double> &frequencies); // Updated to take QVector<double>
    void handleAudioData(const QByteArray &audioData);      // New slot for handling raw audio data
    void onFrequencyDialChanged(int value);

signals:
    void processAudioData(const QByteArray &audioData); // Signal to process audio data in the worker thread

private:
    Ui::MainWindow *ui;
    AudioInput *audioInput;
    AudioWorker *audioWorker;
    QThread *workerThread;
    FrequencyMeter *frequencyMeter;
    QList<QAudioDeviceInfo> audioDevices;

    void populateDeviceList();
    void updateRecordingIndicator(bool recording);
    void initializeAudioInput(const QAudioDeviceInfo &deviceInfo);
};

#endif // MAINWINDOW_H
