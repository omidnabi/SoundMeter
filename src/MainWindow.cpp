#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), workerThread(new QThread(this)), audioWorker(new AudioWorker()), audioInput(nullptr), frequencyMeter(nullptr)
{
    ui->setupUi(this);

    // Register QVector<double> type with Qt
    qRegisterMetaType<QVector<double>>("QVector<double>");

    // Use the frequency meter widget from the UI
    frequencyMeter = ui->frequencyMeter;  // Get the existing frequency meter from the UI

    // Populate the device list
    populateDeviceList();

    // Initialize audio input with the default device
    initializeAudioInput(QAudioDeviceInfo::defaultInputDevice());

    // Set up audio worker and move it to a separate thread
    audioWorker->moveToThread(workerThread);
    connect(workerThread, &QThread::finished, audioWorker, &QObject::deleteLater);
    connect(this, &MainWindow::processAudioData, audioWorker, &AudioWorker::processAudioData);
    connect(audioWorker, &AudioWorker::frequencyAnalysisReady, this, &MainWindow::updateDisplay);

    // Ensure the worker thread starts
    workerThread->start();

    // Connect UI signals
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartRecording);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopRecording);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveRecording);
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDeviceChanged);
    connect(ui->frequencyDial, &QDial::valueChanged, this, &MainWindow::onFrequencyDialChanged);

    // Set up the recording indicator (initially hidden)
    ui->recordingIndicator->setVisible(false);
}

MainWindow::~MainWindow()
{
    workerThread->quit();
    workerThread->wait();
    delete audioWorker;
    delete workerThread;
    delete audioInput;
    delete ui;
}

void MainWindow::populateDeviceList()
{
    // Get the list of available audio devices
    audioDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const QAudioDeviceInfo &deviceInfo : audioDevices) {
        ui->deviceComboBox->addItem(deviceInfo.deviceName());
    }

    // Set the combo box to the default input device
    QAudioDeviceInfo defaultDevice = QAudioDeviceInfo::defaultInputDevice();
    int defaultIndex = audioDevices.indexOf(defaultDevice);

    if (defaultIndex != -1) {
        ui->deviceComboBox->setCurrentIndex(defaultIndex);
        onDeviceChanged(defaultIndex);  // Initialize with the default device
    } else if (!audioDevices.isEmpty()) {
        ui->deviceComboBox->setCurrentIndex(0);  // Fallback to the first device if no default is found
        onDeviceChanged(0);  // Initialize with the first device
    }
}

void MainWindow::initializeAudioInput(const QAudioDeviceInfo &deviceInfo)
{
    if (audioInput) {
        audioInput->stop();
        delete audioInput;
    }

    audioInput = new AudioInput(this);
    audioInput->setDevice(deviceInfo);
    connect(audioInput, &AudioInput::dataReady, this, &MainWindow::handleAudioData);
}

void MainWindow::onDeviceChanged(int index)
{
    if (index >= 0 && index < audioDevices.size()) {
        QAudioDeviceInfo selectedDevice = audioDevices.at(index);
        initializeAudioInput(selectedDevice);
    }
}

void MainWindow::onStartRecording()
{
    if (audioInput) {
        audioInput->start();
        updateRecordingIndicator(true);  // Show recording indicator
    }
}

void MainWindow::onStopRecording()
{
    if (audioInput) {
        audioInput->stop();
        updateRecordingIndicator(false);  // Hide recording indicator
    }
}

void MainWindow::onSaveRecording()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Recording"), "", tr("WAV files (*.wav)"));
    if (fileName.isEmpty()) return;

    QByteArray audioData = audioInput->getAudioBuffer();

    if (audioData.isEmpty()) {
        QMessageBox::warning(this, tr("Save Failed"), tr("No audio data to save."));
        return;
    }

    // Prepare the WAV header
    QByteArray header;
    int dataSize = audioData.size();
    int sampleRate = audioInput->getSampleRate();  // Use the actual sample rate from AudioInput
    int numChannels = 1;     // Mono audio
    int bitsPerSample = 16;  // 16-bit samples
    int byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    int blockAlign = numChannels * (bitsPerSample / 8);
    int subchunk1Size = 16;  // PCM header size
    int audioFormat = 1;     // PCM = 1 (linear quantization)

    int chunkSize = 4 + (8 + subchunk1Size) + (8 + dataSize);

    header.append("RIFF");
    header.append(reinterpret_cast<const char*>(&chunkSize), 4);
    header.append("WAVE");
    header.append("fmt ");
    header.append(reinterpret_cast<const char*>(&subchunk1Size), 4);
    header.append(reinterpret_cast<const char*>(&audioFormat), 2); // PCM format (1)
    header.append(reinterpret_cast<const char*>(&numChannels), 2);
    header.append(reinterpret_cast<const char*>(&sampleRate), 4);
    header.append(reinterpret_cast<const char*>(&byteRate), 4);
    header.append(reinterpret_cast<const char*>(&blockAlign), 2);
    header.append(reinterpret_cast<const char*>(&bitsPerSample), 2);
    header.append("data");
    header.append(reinterpret_cast<const char*>(&dataSize), 4);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(header);  // Write the WAV header first
        file.write(audioData);  // Write the raw audio data
        file.close();
        QMessageBox::information(this, tr("Save Successful"), tr("The recording has been saved successfully."));
    } else {
        QMessageBox::warning(this, tr("Save Failed"), tr("Could not save the recording."));
    }
}

void MainWindow::updateRecordingIndicator(bool recording)
{
    static QTimer* timer = nullptr;
    if (recording) {
        ui->recordingIndicator->setVisible(true);
        if (!timer) {
            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, [=]() {
                ui->recordingIndicator->setVisible(!ui->recordingIndicator->isVisible());
            });
        }
        timer->start(500);  // Toggle visibility every 500ms
    } else {
        if (timer) {
            timer->stop();
        }
        ui->recordingIndicator->setVisible(false);
    }
}

void MainWindow::handleAudioData(const QByteArray &audioData)
{
    emit processAudioData(audioData);  // Emit signal to process audio data in worker thread
}

void MainWindow::updateDisplay(const QVector<double> &frequencies)
{
    if (frequencies.isEmpty()) {
        qDebug() << "Frequency analysis returned empty.";
        return;
    }

    auto maxIt = std::max_element(frequencies.begin(), frequencies.end());
    if (maxIt == frequencies.end()) {
        qDebug() << "No maximum frequency found.";
        return;
    }

    int maxIndex = std::distance(frequencies.begin(), maxIt);
    double maxFrequency = *maxIt;

    int sampleRate = audioInput->getSampleRate();  // Ensure this is correct
    double dominantFrequencyHz = maxIndex * static_cast<double>(sampleRate) / frequencies.size();

    // Update frequency label
    ui->frequencyLabel->setText(QString("Dominant Frequency: %1 Hz").arg(dominantFrequencyHz));

    // Update frequency meter
    frequencyMeter->updateFrequency(dominantFrequencyHz);
}

void MainWindow::onFrequencyDialChanged(int value)
{
    // Update the frequency label
    ui->frequencyDialLabel->setText(QString("Input Frequency: %1 Hz").arg(value));

    // Adjust the sampling rate or perform an action based on the new frequency
    if (audioInput) {
        audioInput->setSampleRate(value); // Adjust based on your actual method
    }
}

// Include the generated moc file for MainWindow
#include "moc_MainWindow.cpp"
