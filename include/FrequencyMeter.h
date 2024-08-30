#ifndef FREQUENCYMETER_H
#define FREQUENCYMETER_H

#include <QWidget>

class FrequencyMeter : public QWidget
{
    Q_OBJECT  // Ensure this macro is present

public:
    explicit FrequencyMeter(QWidget *parent = nullptr);

    void updateFrequency(double frequency);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double currentFrequency;  // The current frequency to display
};

#endif // FREQUENCYMETER_H

