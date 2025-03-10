#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <QApplication>
#include <QDialog>
#include <QPoint>
#include <QMouseEvent>

class CustomMessageBox : public QDialog
{
    Q_OBJECT

   public:
    explicit CustomMessageBox(QWidget *parent = nullptr);
    ~CustomMessageBox() override = default;

   public:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
   private slots:
    void updateBackgroundGradient();    // 新增槽函数，用于更新背景
   private:
    QPoint click_pos_;
    QTimer *animation_timer_;    // 动画定时器
    int hue_;                    // 色调值，用于渐变动画
};

#endif
