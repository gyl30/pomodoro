#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QStyle>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QColor>
#include <QMessageBox>
#include "message_box.h"

CustomMessageBox::CustomMessageBox(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("定时提醒");
    setFixedSize(300, 150);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    auto *label = new QLabel("运动时间到啦", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; color: #333; padding: 10px;");

    // 添加确认按钮
    auto *confirm = new QPushButton("好的", this);
    confirm->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #388e3c;"
        "}");
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(confirm);
    connect(confirm, &QPushButton::clicked, this, &QDialog::accept);

    // 初始化背景渐变动画
    hue_ = 180;                     // 初始色调，与 Widget 一致
    setAutoFillBackground(true);    // 启用背景自动填充
    updateBackgroundGradient();     // 设置初始背景

    // animation_timer_ = new QTimer(this);
    // connect(animation_timer_, &QTimer::timeout, this, &CustomMessageBox::updateBackgroundGradient);
    // animation_timer_->start(50);    // 每 50ms 更新一次，与 Widget 一致
    gl_background_ = new GLBackgroundWidget(this);
    gl_background_->setGeometry(0, 0, width(), height());
    gl_background_->lower();
}
void CustomMessageBox::updateBackgroundGradient()
{
    hue_ = (hue_ + 1) % 360;    // 色调循环

    QColor color1 = QColor::fromHsv(hue_, 100, 255);                 // 顶部颜色
    QColor color2 = QColor::fromHsv((hue_ + 60) % 360, 100, 255);    // 底部颜色

    QPalette palette;
    QLinearGradient gradient(0, 0, 0, height());    // 从上到下渐变
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(1.0, color2);

    palette.setBrush(QPalette::Window, gradient);
    setPalette(palette);
}

void CustomMessageBox::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        click_pos_ = e->pos();
    }
}
void CustomMessageBox::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) != 0U)
    {
        move(e->pos() + pos() - click_pos_);
    }
}
