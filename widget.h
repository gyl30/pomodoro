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

   private:
    QPoint screenCenter();

   private:
    QTimer *timer_ = nullptr;
    QSystemTrayIcon *tray_ = nullptr;
    QTimer *tooltip_update_timer_;
    QTimer *background_animation_timer_;
    int hue_;
    QSpinBox *time_spinbox_ = nullptr;
    QPushButton *start_button_ = nullptr;
};
#endif    // WIDGET_H
