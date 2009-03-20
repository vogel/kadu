#ifndef KADU_IMAGE_DIALOG_H
#define KADU_IMAGE_DIALOG_H

#include "base.h"

class ImageDialog : public QFileDialog
{
public:
	ImageDialog(QWidget *parent);
};

#endif