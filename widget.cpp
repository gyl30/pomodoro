#include "widget.h"
#include "message_box.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *label = new QLabel("设置时间间隔 (秒):", this);
    label->setStyleSheet("font-size: 16px; color: #333; margin-bottom: 10px;");
    layout->addWidget(label);

    time_spinbox_ = new QSpinBox(this);
    time_spinbox_->setStyleSheet(
        "font-size: 14px; padding: 8px; border: 2px "
        "solid #4CAF50; border-radius: 5px;");
    time_spinbox_->setRange(1, 3600);    // 设置时间范围 1 到 3600 秒
    layout->addWidget(time_spinbox_);

    start_button_ = new QPushButton("开始计时", this);
    start_button_->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #388e3c;"
        "}");
    layout->addWidget(start_button_);

    setLayout(layout);

    // 创建托盘图标（使用系统默认图标）
    tray_ = new QSystemTrayIcon(nullptr);
    auto *tray_menu = new QMenu(this);
    auto *exit_action = new QAction("退出", this);
    auto *open_action = new QAction("启动器", this);
    tray_menu->addAction(open_action);
    tray_menu->addAction(exit_action);
    tray_->setContextMenu(tray_menu);
    connect(tray_, &QSystemTrayIcon::activated, this, &Widget::tray_clicked, Qt::UniqueConnection);
    connect(exit_action, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(open_action, &QAction::triggered, this, [this]() { this->show(); });
    tray_->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    tray_->setVisible(true);
    tray_->show();


    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Widget::showTrayNotification);

    connect(start_button_, &QPushButton::clicked, this, &Widget::startTimer);
    setFixedSize(300, 150);
}
void Widget::startTimer()
{
    if (!this->isHidden())
    {
        this->hide();
    }
    int interval = time_spinbox_->value() * 1000;
    timer_->start(interval);
}

void Widget::stopTimer() { timer_->stop(); }

void Widget::tray_clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (!this->isHidden())
    {
        return;
    }
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger)
    {
        showNormal();
        activateWindow();
    }
}
QPoint Widget::screenCenter()
{
    QScreen *screen = this->screen();
    QRect screen_geometry = screen->availableGeometry();
    int x = screen_geometry.center().x() - (width() / 2);
    int y = screen_geometry.center().y() - (height() / 2);
    return {x, y};
}
void Widget::showTrayNotification()
{
    stopTimer();
    CustomMessageBox msg_box(this);
    auto pos = screenCenter();
    msg_box.move(pos);
    msg_box.exec();
    startTimer();
}
