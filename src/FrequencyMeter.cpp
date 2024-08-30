#include "FrequencyMeter.h"
#include <QPainter>
#include <QRectF>

FrequencyMeter::FrequencyMeter(QWidget *parent) : QWidget(parent), currentFrequency(0)
{
    setMinimumHeight(100);
}

void FrequencyMeter::updateFrequency(double frequency)
{
    currentFrequency = frequency;
    update();  // Trigger a repaint
}

void FrequencyMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);  // Prevent unused parameter warning

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Clear the background with a white color
    painter.fillRect(rect(), Qt::white);

    // Define the maximum frequency range that you expect to visualize
    double maxFrequency = 22050.0;  // Maximum frequency for half the sample rate (Nyquist limit)

    // Calculate the bar height based on the current frequency
    int barHeight = static_cast<int>((currentFrequency / maxFrequency) * height());

    // Ensure the bar height does not exceed the widget's height
    barHeight = qBound(0, barHeight, height());

    // Draw the frequency bar from the bottom up
    QRectF barRect(0, height() - barHeight, width(), barHeight);
    painter.setBrush(Qt::blue);
    painter.drawRect(barRect);

    // Draw text label to show the current frequency
    QString frequencyText = QString::number(static_cast<int>(currentFrequency)) + " Hz";
    painter.setPen(Qt::black);  // Set text color to black
    painter.drawText(rect(), Qt::AlignCenter, frequencyText);
}

#include "moc_FrequencyMeter.cpp"
