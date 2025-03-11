#include "widget.h"
#include "message_box.h"
#include "glbackgroundwidget.h"
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
#include <QDebug>

static QPoint screenCenter()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen == nullptr)
    {
        return {0, 0};
    }
    QRect screen_geometry = screen->availableGeometry();
    int x = screen_geometry.center().x();
    int y = screen_geometry.center().y();
    return {x, y};
}
class CustomSpinBox : public QSpinBox
{
   public:
    explicit CustomSpinBox(QWidget *parent = nullptr) : QSpinBox(parent) {}

   protected:
    void wheelEvent(QWheelEvent *event) override
    {
        int step = singleStep();
        int val = value();
        int base = (val / step) * step;

        if (event->angleDelta().y() > 0)
        {
            setValue(base + step);
        }
        else
        {
            setValue(base - step);
        }
        event->accept();
    }
};
Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("设置时间间隔 (分钟):", this);
    label->setStyleSheet("font-size: 16px; color: #333; margin-bottom: 10px;");
    layout->addWidget(label);

    time_spinbox_ = new CustomSpinBox(this);
    QFont font = time_spinbox_->font();
    font.setPointSize(14);    // 设置字体大小，与 label 协调
    time_spinbox_->setSingleStep(5);
    time_spinbox_->setFont(font);
    time_spinbox_->setStyleSheet(
        "QSpinBox {"
        "    padding: 6px 10px; border: 2px solid #4CAF50; border-radius: 8px;"
        "    background-color: #ffffff; color: #333;"
        "    min-height: 30px;"    // 确保高度足够
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "    width: 0px;"
        "    background-color: #e0f7fa;"
        "    border-radius: 4px;"
        "}"
        "QSpinBox::up-arrow, QSpinBox::down-arrow {"
        "    width: 10px; height: 10px;"
        "}");
    time_spinbox_->setRange(1, 60);    // 设置时间范围 1 到 60 分钟
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
    center_pos_ = screenCenter();
    move(center_pos_);
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
        move(center_pos_);
        showNormal();
        activateWindow();
    }
}
void Widget::updateBackgroundGradient()
{
    hue_ = (hue_ + 1) % 360;

    QColor color1 = QColor::fromHsv(hue_, 150, 255);
    QColor color2 = QColor::fromHsv((hue_ + 60) % 360, 150, 255);

    // 创建渐变位置也动态变化
    static int offset = 0;
    offset = (offset + 1) % width();    // 或使用 height() 更换方向

    QPalette palette;
    QLinearGradient gradient(0, 0, offset, height());    // 渐变方向横向“滑动”
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(1.0, color2);

    palette.setBrush(QPalette::Window, gradient);
    setPalette(palette);
}
void Widget::showTrayNotification()
{
    CustomMessageBox msg_box(this);
    msg_box.move(center_pos_);
    msg_box.exec();
}
void Widget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        click_pos_ = e->pos();
    }
}
void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) != 0U)
    {
        move(e->pos() + pos() - click_pos_);
    }
}
void Widget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        hide();
    }
    QWidget::mouseDoubleClickEvent(e);
}
