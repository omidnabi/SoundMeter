#include "AudioInput.h"
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QDebug>

AudioInput::AudioInput(QObject *parent)
    : QObject(parent), audioInput(nullptr), audioDevice(nullptr), sampleRate(10000)  // Initialize sampleRate
{
    // Initialize with the default device
    QAudioDeviceInfo defaultDevice = QAudioDeviceInfo::defaultInputDevice();
    setDevice(defaultDevice);
}

AudioInput::~AudioInput()
{
    stop();  // Ensure audio input is stopped properly before deletion
    if (audioInput) {
        delete audioInput;  // Only delete if it has not been deleted already
        audioInput = nullptr;
    }
}

void AudioInput::start()
{
    if (audioInput) {
        audioDevice = audioInput->start();
        if (audioDevice) {
            connect(audioDevice, &QIODevice::readyRead, this, &AudioInput::handleAudioInput);
        } else {
            qDebug() << "Failed to start audio input: audioDevice is nullptr";
        }
    }
}

void AudioInput::stop()
{
    if (audioInput) {
        if (audioDevice) {
            disconnect(audioDevice, &QIODevice::readyRead, this, &AudioInput::handleAudioInput);
            audioDevice = nullptr;
        }

        audioInput->stop();
    }
}

void AudioInput::setDevice(const QAudioDeviceInfo &deviceInfo)
{
    if (audioInput) {
        stop();
        delete audioInput;
        audioInput = nullptr;
    }

    QAudioFormat format;
    format.setSampleRate(sampleRate);  // Use current sample rate
    format.setChannelCount(1);     // Mono channel
    format.setSampleSize(16);      // 16-bit samples
    format.setCodec("audio/pcm");  // PCM codec
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Requested format not supported - using the nearest supported format.";
        format = deviceInfo.nearestFormat(format);
    }

    audioInput = new QAudioInput(deviceInfo, format, this);
    qDebug() << "Audio device set to:" << deviceInfo.deviceName();
}

void AudioInput::setSampleRate(int frequency)
{
    sampleRate = frequency;  // Update the sample rate
}

int AudioInput::getSampleRate() const
{
    return sampleRate;  // Return the current sample rate
}

QByteArray AudioInput::getAudioBuffer() const
{
    return audioBuffer;
}

void AudioInput::handleAudioInput()
{
    if (audioDevice) {
        QByteArray newData = audioDevice->readAll();  // Read new audio data
        audioBuffer.append(newData);  // Append new audio data to the buffer
        emit dataReady(audioBuffer);  // Emit signal with the new audio data
    } else {
        qDebug() << "Audio device is null, no data to read.";
    }
}

// Include the generated MOC file for AudioInput
#include "moc_AudioInput.cpp"
