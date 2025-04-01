#include "photolabel.h"
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QFileInfo>
#include <QMessageBox>

PhotoLabel::PhotoLabel(QWidget* parent) :QLabel(parent)
{
	initWidget();
}

void PhotoLabel::initWidget()
{
	//初始化右键菜单
	m_menu = new QMenu(this);
	QAction* loadImage = new QAction;
	loadImage->setText(tr("&Open new..."));
	connect(loadImage, &QAction::triggered, this, &PhotoLabel::onSelectImage);
	m_menu->addAction(loadImage);

	QAction* pasteImage = new QAction;
	pasteImage->setText(tr("&Paste image"));
	connect(pasteImage, &QAction::triggered, this, &PhotoLabel::onPasteImage);
	m_menu->addAction(pasteImage);
	m_menu->addSeparator();

	QAction* zoomInAction = new QAction;
    zoomInAction->setText(tr("Zoom in &+"));
	connect(zoomInAction, &QAction::triggered, this, &PhotoLabel::onZoomInImage);
	m_menu->addAction(zoomInAction);

	QAction* zoomOutAction = new QAction;
    zoomOutAction->setText(tr("Zoom out &-"));
	connect(zoomOutAction, &QAction::triggered, this, &PhotoLabel::onZoomOutImage);
	m_menu->addAction(zoomOutAction);

	QAction* presetAction = new QAction;
	presetAction->setText(tr("&Reset"));
	connect(presetAction, &QAction::triggered, this, &PhotoLabel::onPresetImage);
	m_menu->addAction(presetAction);
	m_menu->addSeparator();
	/*
	QAction *clearAction = new QAction;
	clearAction->setText("Clear");
	connect(clearAction, &QAction::triggered, this, &PhotoLabel::clearShow);
	m_menu->addAction(clearAction);
	*/
}

void PhotoLabel::openFile(QString path)
{
	if (path.isEmpty())
	{
		return;
	}

	if (!m_image.load(path))
	{
		QMessageBox::warning(this, tr("Error"), tr("Cannot load file!"));
		return;
	}

	m_zoomValue = 1.0;
	m_xPtInterval = 0;
	m_yPtInterval = 0;

	m_localFileName = path;
	emit this->imageLoadSuccess();
	update();
}

void PhotoLabel::clearShow()
{
	m_localFileName = "";
	m_image = QImage();
	this->clear();
}

void PhotoLabel::setImage(QImage& img)
{
	if (img.isNull())
	{
		return;
	}

	m_zoomValue = 1.0;
	m_xPtInterval = 0;
	m_yPtInterval = 0;

	m_localFileName = "";
	m_image = img.copy(0, 0, img.width(), img.height());
	emit imageLoadSuccess();
	update();
}

void PhotoLabel::openAction()
{
	this->onSelectImage();
}

void PhotoLabel::pasteAction()
{
	this->onPasteImage();
}

const QImage& PhotoLabel::getImage()
{
	return m_image;
}

void PhotoLabel::paintEvent(QPaintEvent* event)
{
	if (m_image.isNull())
		return QWidget::paintEvent(event);

	QPainter painter(this);

	// 根据窗口计算应该显示的图片的大小
	int width = qMin(m_image.width(), this->width());
	int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
	height = qMin(height, this->height());
	width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

	// 平移
	painter.translate(this->width() / 2 + m_xPtInterval, this->height() / 2 + m_yPtInterval);

	// 缩放
	painter.scale(m_zoomValue, m_zoomValue);

	// 绘制图像
	QRect picRect(-width / 2, -height / 2, width, height);
	painter.drawImage(picRect, m_image);

	QWidget::paintEvent(event);
}

void PhotoLabel::wheelEvent(QWheelEvent* event)
{
	int value = event->angleDelta().y() / 15;
	if (value < 0)  //放大
		onZoomInImage();
	else            //缩小
		onZoomOutImage();

	update();
}

void PhotoLabel::mousePressEvent(QMouseEvent* event)
{
	m_oldPos = event->pos();
	m_pressed = true;
	this->setCursor(Qt::ClosedHandCursor); //设置鼠标样式
}

void PhotoLabel::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_pressed)
		return QWidget::mouseMoveEvent(event);

	QPoint pos = event->pos();
	int xPtInterval = pos.x() - m_oldPos.x();
	int yPtInterval = pos.y() - m_oldPos.y();

	m_xPtInterval += xPtInterval;
	m_yPtInterval += yPtInterval;

	m_oldPos = pos;
	update();
}

void PhotoLabel::mouseReleaseEvent(QMouseEvent*/*event*/)
{
	m_pressed = false;
	this->setCursor(Qt::ArrowCursor); //设置鼠标样式
}

void PhotoLabel::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
	else
	{
		event->ignore();
	}
}

void PhotoLabel::dragMoveEvent(QDragMoveEvent* event)
{

}

void PhotoLabel::dropEvent(QDropEvent* event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.empty())
		return;

	QString fileName = urls.first().toLocalFile();
	QFileInfo info(fileName);
	if (!info.isFile())
		return;

	this->openFile(fileName);
}

void PhotoLabel::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint pos = event->pos();
	pos = this->mapToGlobal(pos);
	m_menu->exec(pos);
}

void PhotoLabel::onSelectImage()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open a image file"), "./", tr("Images (*.bmp *.png *.jpg *.jpeg *.gif *.tiff)\nAll files (*.*)"));
	if (path.isEmpty())
		return;

	openFile(path);
}

void PhotoLabel::onPasteImage()
{
	QClipboard* clipboard = QApplication::clipboard();
	QImage img = clipboard->image();
	if (img.isNull())
	{
		return;
	}

	this->setImage(img);
}

void PhotoLabel::onZoomInImage()
{
	m_zoomValue += 0.05;
	update();
}

void PhotoLabel::onZoomOutImage()
{
	m_zoomValue -= 0.05;
	if (m_zoomValue <= 0)
	{
		m_zoomValue = 0.05;
		return;
	}

	update();
}

void PhotoLabel::onPresetImage()
{
	m_zoomValue = 1.0;
	m_xPtInterval = 0;
	m_yPtInterval = 0;
	update();
}
