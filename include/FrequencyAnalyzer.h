#ifndef FREQUENCYANALYZER_H
#define FREQUENCYANALYZER_H

#include <QByteArray>
#include <QVector>

class FrequencyAnalyzer
{
public:
    FrequencyAnalyzer();
    QVector<double> analyze(const QByteArray &audioData);
};

#endif // FREQUENCYANALYZER_H
