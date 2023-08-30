//
// Created by RGAA on 2023/8/30.
//

#include "RoundRectWidget.h"
#include <QPainter>

namespace rgaa {

    RoundRectWidget::RoundRectWidget(int color, int radius, QWidget* parent) : QWidget(parent) {
        this->bg_color_ = color;
        this->radius_ = radius;
    }

    void RoundRectWidget::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(bg_color_)));
        painter.drawRoundedRect(0, 0, this->width(), this->height(), radius_, radius_);
    }

}