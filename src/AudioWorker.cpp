#include "AudioWorker.h"
#include <QDebug>




AudioWorker::AudioWorker(QObject *parent) : QObject(parent)
{
}

void AudioWorker::processAudioData(const QByteArray &audioData)
{
    int sampleSize = 16;  // 16-bit samples
    int numChannels = 1;  // Mono audio
    int numSamples = audioData.size() / (sampleSize / 8 * numChannels);

    QVector<double> frequencies(numSamples / 2);  // FFT results up to Nyquist frequency

    if (numSamples <= 0) {
        qWarning() << "Not enough audio data for analysis.";
        emit frequencyAnalysisReady(frequencies);
        return;
    }

    QVector<std::complex<double>> complexData(numSamples);
    const qint16 *samples = reinterpret_cast<const qint16 *>(audioData.constData());

    for (int i = 0; i < numSamples; ++i) {
        complexData[i] = std::complex<double>(samples[i], 0.0);
    }

    fft(complexData);

    for (int i = 0; i < frequencies.size(); ++i) {
        frequencies[i] = std::abs(complexData[i]);
    }

    emit frequencyAnalysisReady(frequencies);  // Emit the signal with the calculated frequencies
}

void AudioWorker::fft(QVector<std::complex<double>> &data)
{
    int n = data.size();
    if (n <= 1) return;

    QVector<std::complex<double>> even = data.mid(0, n / 2);
    QVector<std::complex<double>> odd = data.mid(n / 2);

    fft(even);
    fft(odd);

    for (int k = 0; k < n / 2; ++k) {
        std::complex<double> t = std::polar(1.0, -2 * M_PI * k / n) * odd[k];
        data[k] = even[k] + t;
        data[k + n / 2] = even[k] - t;
    }
}
