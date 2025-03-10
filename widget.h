#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QSpinBox>
#include <QPushButton>

class Widget : public QWidget
{
    Q_OBJECT

   public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override = default;

    // public:
    //  void closeEvent(QCloseEvent *event) override;
    //  void hideEvent(QHideEvent *event) override;
   private slots:
    void startTimer();
    void stopTimer();
    void showTrayNotification();
    void updateBackgroundGradient();
    void trayClicked(QSystemTrayIcon::ActivationReason);
    void updateTrayToolTip();
    // 定义scaleFactor属性
   public:
    void setScaleFactor(qreal scale)
    {
        scaleFactor = scale;
        updateIcon();
    }

    qreal getScaleFactor() const { return scaleFactor; }

   private:
    void updateIcon();
    void updateHeartbeat();
    QPoint screenCenter();

   private:
    QTimer *timer_ = nullptr;
    QSystemTrayIcon *tray_ = nullptr;
    QTimer *tooltip_update_timer_;
    QTimer *background_animation_timer_;
    QTimer *heartbeat_timer_;
    bool heartbeat_state_ = false;
    int hue_;
    QSpinBox *time_spinbox_ = nullptr;
    QPushButton *start_button_ = nullptr;
    qreal scaleFactor;
    Q_PROPERTY(qreal scaleFactor READ getScaleFactor WRITE setScaleFactor)
};
#endif    // WIDGET_H
