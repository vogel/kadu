#ifndef KADU_IMAGE_WIDGET_H
#define KADU_IMAGE_WIDGET_H

#include "base.h"

class KADUAPI ImageWidget : public QWidget
{
	QImage Image;

protected:
	virtual void paintEvent(QPaintEvent *e);

public:
	ImageWidget(QWidget *parent);
	ImageWidget(const QByteArray &image, QWidget *parent);
	void setImage(const QByteArray &image);
	void setImage(const QPixmap &image);

};

#endif