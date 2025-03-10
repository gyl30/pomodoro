#include "widget.h"
#include "message_box.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QMenu>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QPalette>
#include <QLinearGradient>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *label = new QLabel("设置时间间隔 (分钟):", this);
    label->setStyleSheet("font-size: 16px; color: #333; margin-bottom: 10px;");
    layout->addWidget(label);

    time_spinbox_ = new QSpinBox(this);
    time_spinbox_->setStyleSheet(
        "QSpinBox {"
        "    font-size: 16px; padding: 6px 10px; border: 2px solid #4CAF50; border-radius: 8px;"
        "    background-color: #ffffff; color: #333;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "    width: 0px;"
        "    background-color: #e0f7fa;"
        "    border-radius: 4px;"
        "}"
        "QSpinBox::up-arrow, QSpinBox::down-arrow {"
        "    width: 10px; height: 10px;"
        "}");
    time_spinbox_->setRange(5, 3600);    // 设置时间范围 5 到 3600 秒
    time_spinbox_->setSingleStep(5);
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

    // 设置背景渐变色
    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());    // 从上到下渐变
    gradient.setColorAt(0.0, QColor("#fefcea"));    // 起始颜色（淡蓝）
    gradient.setColorAt(1.0, QColor("#f1da36"));    // 结束颜色（浅青）
    palette.setBrush(QPalette::Window, gradient);
    setAutoFillBackground(true);
    setPalette(palette);
    // 创建托盘图标（使用系统默认图标）
    tray_ = new QSystemTrayIcon(nullptr);
    auto *tray_menu = new QMenu(this);
    auto *exit_action = new QAction("退出", this);
    auto *open_action = new QAction("启动器", this);
    tray_menu->addAction(open_action);
    tray_menu->addAction(exit_action);
    tray_->setContextMenu(tray_menu);
    connect(tray_, &QSystemTrayIcon::activated, this, &Widget::trayClicked, Qt::UniqueConnection);
    connect(exit_action, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(open_action, &QAction::triggered, this, [this]() { this->show(); });
    heartbeat_timer_ = new QTimer(this);
    heartbeat_timer_->start(1500);
    connect(heartbeat_timer_, &QTimer::timeout, this, &Widget::updateHeartbeat);
    updateHeartbeat();
    tray_->setVisible(true);
    tray_->show();

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &Widget::showTrayNotification);

    tooltip_update_timer_ = new QTimer(this);
    connect(tooltip_update_timer_, &QTimer::timeout, this, &Widget::updateTrayToolTip);
    tooltip_update_timer_->start(500);
    hue_ = 180;
    background_animation_timer_ = new QTimer(this);
    connect(background_animation_timer_, &QTimer::timeout, this, &Widget::updateBackgroundGradient);
    background_animation_timer_->start(50);
    connect(start_button_, &QPushButton::clicked, this, &Widget::startTimer);
    setFixedSize(300, 150);
    move(screenCenter());
}
void Widget::updateTrayToolTip()
{
    int remaining_ms = timer_->remainingTime();
    if (remaining_ms > 0)
    {
        int seconds = remaining_ms / 1000;
        int minutes = seconds / 60;
        int secs = seconds % 60;

        QString tip = QString("距离提醒还有：%1分%2秒").arg(minutes).arg(secs, 2, 10, QChar('0'));
        tray_->setToolTip(tip);
    }
}
void Widget::updateHeartbeat()
{
    auto *animation = new QPropertyAnimation(this, "scaleFactor");
    animation->setDuration(1000);
    animation->setKeyValueAt(0, 1.0);
    animation->setKeyValueAt(0.2, 0.8);
    animation->setKeyValueAt(1.0, 1.0);
    connect(animation, &QPropertyAnimation::valueChanged, this, &Widget::updateIcon);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::updateIcon()
{
    // 绘制心形图标
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    // 设置支持Emoji的字体
    painter.setFont(QFont("Segoe UI Emoji", 24 * scaleFactor));

    // 动态调整颜色（从浅红到深红）
    QColor heartColor = QColor::fromHslF(0, 1.0, 0.5 + ((scaleFactor - 0.8) * 0.25));
    painter.setPen(heartColor);

    // 绘制心形Emoji
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "❤️");

    // 更新托盘图标
    tray_->setIcon(QIcon(pixmap));
}
void Widget::startTimer()
{
    if (!this->isHidden())
    {
        this->hide();
    }
    int interval = time_spinbox_->value() * 1000 * 60;
    timer_->start(interval);
}

void Widget::stopTimer() { timer_->stop(); }

void Widget::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
    if (!this->isHidden())
    {
        return;
    }
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger)
    {
        move(screenCenter());
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
void Widget::updateBackgroundGradient()
{
    hue_ = (hue_ + 1) % 360;

    QColor color1 = QColor::fromHsv(hue_, 100, 255);
    QColor color2 = QColor::fromHsv((hue_ + 60) % 360, 100, 255);

    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(1.0, color2);

    palette.setBrush(QPalette::Window, gradient);
    setPalette(palette);
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
