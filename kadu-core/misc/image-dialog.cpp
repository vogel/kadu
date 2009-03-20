#include "image-dialog.h"

ImageDialog::ImageDialog(QWidget *parent)
	: QFileDialog(parent)
{
	setFilter(qApp->translate("ImageDialog", "Images")+" (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP)");
}