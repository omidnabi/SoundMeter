#include "FrequencyAnalyzer.h"
#include <QDebug>

FrequencyAnalyzer::FrequencyAnalyzer()
{
}

QVector<double> FrequencyAnalyzer::analyze(const QByteArray &audioData)
{
    // Implement any non-FFT analysis if needed or leave it for future use
    QVector<double> result;
    // You could leave this function empty or add other analysis methods here
    return result;
}
