#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H
#include <math.h>  // or #include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include <QObject>
#include <QVector>
#include <complex>

class AudioWorker : public QObject
{
    Q_OBJECT
public:
    explicit AudioWorker(QObject *parent = nullptr);

signals:
    void frequencyAnalysisReady(const QVector<double> &frequencies);  // Signal emitted when frequency analysis is complete

public slots:
    void processAudioData(const QByteArray &audioData);  // Slot to process incoming audio data

private:
    void fft(QVector<std::complex<double>> &data);  // Performs FFT on the data
};

#endif // AUDIOWORKER_H
