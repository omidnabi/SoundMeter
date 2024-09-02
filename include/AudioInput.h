#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QObject>
#include <QAudioInput>
#include <QIODevice>
#include <QByteArray>
#include <QAudioDeviceInfo>

class AudioInput : public QObject
{
    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    ~AudioInput();

    void start();  // Start recording audio
    void stop();   // Stop recording audio
    void setDevice(const QAudioDeviceInfo &deviceInfo);  // Set the audio input device
    QByteArray getAudioBuffer() const;  // Retrieve the audio buffer
    void setSampleRate(int frequency);
    int getSampleRate() const;  // Declare the getSampleRate function here

    int sampleRate;

signals:
    void dataReady(const QByteArray &data);  // Signal emitted when new audio data is ready

private slots:
    void handleAudioInput();  // Slot to handle audio input data

private:
    QAudioInput *audioInput;
    QIODevice *audioDevice;
    QByteArray audioBuffer;  // Buffer to store audio data
};

#endif // AUDIOINPUT_H
