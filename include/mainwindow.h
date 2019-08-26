#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QUuid>

#include "screenshotprocess.h"
#include "meminfoprocess.h"
#include "stacktraceprocess.h"
#include "stacktracemodel.h"
#include "stacktraceproxymodel.h"
#include "addressprocess.h"
#include "startappprocess.h"
#include "fixedscrollarea.h"
#include "interactivechartview.h"

namespace Ui {
class MainWindow;
}

class QTreeWidgetItem;
class QStandardItemModel;
class QGraphicsPixmapItem;
class QProgressDialog;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void LoadSettings();
    void SaveSettings();

    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void Print(const QString& str);
    void SaveToFile(QFile *file);
    int LoadFromFile(QFile *file);
    QString GetLastOpenDir() const;
    QString GetLastSymbolDir() const;

    void ConnectionFailed();

    int GetScreenshotIndex(const QPointF& pos) const;
    void ShowScreenshotAt(int index);
    void HideToolTips();
    void UpdateMemInfoRange();
    QString TryAddNewAddress(const QString& lib, const QString& addr);
    void ShowCallStack(const QModelIndex& index);
    void ShowSummary();
    void ReadSMapsFile(QFile* file);

private slots:
    void FixedUpdate();

    void OnTimeSelectionChange(const QPointF& pos);
    void OnSyncScroll(QtCharts::QChartView* sender, int prevMouseX, int delta);
    void OnStackTableViewContextMenu(const QPoint & pos);
    void OnStackTableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void StartAppProcessFinished(AdbProcess* process);
    void StartAppProcessErrorOccurred();
    void MemInfoProcessFinished(AdbProcess* process);
    void MemInfoProcessErrorOccurred();
    void ScreenshotProcessFinished(AdbProcess* process);
    void ScreenshotProcessErrorOccurred();
    void StacktraceDataReceived();
    void StacktraceConnectionLost();
    void AddressProcessFinished(AdbProcess* process);
    void AddressProcessErrorOccurred();

    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionStat_SMaps_triggered();
    void on_actionAbout_triggered();
    void on_sdkPushButton_clicked();
    void on_launchPushButton_clicked();
    void on_chartScaleHSlider_valueChanged(int value);
    void on_symbloPushButton_clicked();
    void on_addr2LinePushButton_clicked();
    void on_pythonPushButton_clicked();
    void on_configPushButton_clicked();
    void on_memSizeComboBox_currentIndexChanged(int index);
    void on_libraryComboBox_currentIndexChanged(int index);
    void on_allocComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QProgressDialog *progressDialog_;
    QStandardItemModel *callStackModel_;
    StackTraceModel *stacktraceModel_;
    StackTraceProxyModel *stacktraceProxyModel_;
    QHash<QUuid, QVector<QString>> callStackMap_;
    QSet<QString> libraries_;
    QString adbPath_;
    QString appPid_;
    QString lastOpenDir_;
    QString lastSymbolDir_;
    QTimer* mainTimer_;
    int time_ = 0;

    // adb shell monkey -p packagename -c android.intent.category.LAUNCHER 1
    StartAppProcess *startAppProcess_;

    // screenshot
    ScreenshotProcess *screenshotProcess_;
    QVector<QGraphicsPixmapItem*> screenshotItems_;
    int lastScreenshotTime_ = 0;
    QVector<QPair<int, QByteArray>> screenshots_;

    // stacktrace process
    StackTraceProcess *stacktraceProcess_;
    int stacktraceRetryCount_ = 0;

    // address process
    QVector<AddressProcess*> addrProcesses_;
    // <dllname, <func address, func name>>
    QHash<QString, QHash<QString, QString>> symbloMap_;
    // <mem address, time>
    QHash<QString, int> freeAddrMap_;

    // meminfo process
    MemInfoProcess* memInfoProcess_;
    int maxMemInfoValue_ = 128;
    QVector<QtCharts::QLineSeries*> memInfoSeries_;
    QtCharts::QValueAxis *memInfoAxisX_;
    QtCharts::QValueAxis *memInfoAxisY_;
    QtCharts::QChart *memInfoChart_;
    InteractiveChartView *memInfoChartView_;

    // charts
    FixedScrollArea* scrollArea_;

    struct SMapsSection {
        qint32 VMSize = 0;
        qint32 RSS = 0;
        qint32 PSS = 0;
        qint32 SharedClean = 0;
        qint32 SharedDirty = 0;
        qint32 PrivateClean = 0;
        qint32 PrivateDirty = 0;
    };
    QHash<QString, SMapsSection> sMapsSections_;

    bool isCapturing_ = false;
    bool isConnected_ = false;
};

#endif // MAINWINDOW_H
