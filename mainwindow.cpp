#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <windows.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::initData
 * 加载初始化数据
 */
void MainWindow::initData()
{
    DISPLAY_DEVICEA DispDev;
    int i = 0;
    ZeroMemory(&DispDev, sizeof(DispDev));
    DispDev.cb = sizeof(DispDev);
    char DeviceName[32] = {0};

    // 找到所有的显示器
    for (i = 0; EnumDisplayDevicesA(NULL, i, &DispDev, 0); i++)
    {
        // 注释掉的是主显示器
        if(/*DispDev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE && */DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
        {
            qDebug() << "Display Name: " << DispDev.DeviceName << "Display key: " << DispDev.DeviceKey << "Display ID: " << DispDev.DeviceID << "Display status: " << DispDev.StateFlags;
            ui->comboBox->addItem(QString("屏幕%1").arg(i + 1), DispDev.DeviceName);
            ui->textEdit->append(DispDev.DeviceName);
        }
    }
}

/**
 * @brief MainWindow::on_pushButton_clicked
 * 确定按钮
 */
void MainWindow::on_pushButton_clicked()
{
    if(ui->comboBox->currentIndex() < 0) return;

    QString displayName = ui->comboBox->itemData(ui->comboBox->currentIndex()).toString();
    if(!displayName.isEmpty())
    {
        // 根据上面拿到的设备名来调整分辨率
        DEVMODEA lpDevMode;
        ZeroMemory(&lpDevMode, sizeof(lpDevMode));
        lpDevMode.dmSize = sizeof(lpDevMode);
        if(EnumDisplaySettingsExA(displayName.toStdString().c_str(), ENUM_CURRENT_SETTINGS, &lpDevMode, 0))
        {
            lpDevMode.dmPelsWidth	= 1920;
            lpDevMode.dmPelsHeight	= 1080;
            // 先修改注册表
            LONG result = ChangeDisplaySettingsA(&lpDevMode, CDS_FULLSCREEN);
            if (result == DISP_CHANGE_SUCCESSFUL)
            {
                // 使用CDS_UPDATEREGISTRY表示次修改是持久的
                ChangeDisplaySettingsExA(displayName.toStdString().c_str(), &lpDevMode, NULL, CDS_UPDATEREGISTRY, NULL);
            }
            else
            {
                // 修改失败，恢复原有设置
                ChangeDisplaySettingsA(NULL, 0);
            }
        }
    }
}

