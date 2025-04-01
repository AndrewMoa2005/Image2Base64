#ifndef PHOTOLABEL_H
#define PHOTOLABEL_H

#include <QObject>
#include <QLabel>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDropEvent>

class PhotoLabel : public QLabel
{
	Q_OBJECT

public:
	explicit PhotoLabel(QWidget* parent = nullptr);

	void openFile(QString);     //打开图片文件
	void clearShow();           //清空显示
	void setImage(QImage&);     //设置图片
	void openAction();          //调用打开文件对话框
	void pasteAction();         //粘贴来自剪贴板的图片
	const QImage& getImage();   //调用存储的图片数据

signals:
	// 图片加载成功信号
	void imageLoadSuccess();

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;   //右键菜单
	void paintEvent(QPaintEvent* event) override;               //QPaint画图
	void wheelEvent(QWheelEvent* event) override;               //鼠标滚轮滚动
	void mousePressEvent(QMouseEvent* event) override;          //鼠标摁下
	void mouseMoveEvent(QMouseEvent* event) override;           //鼠标松开
	void mouseReleaseEvent(QMouseEvent* event) override;        //鼠标发射事件
	//拖放文件
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private slots:
	void initWidget();      //初始化
	void onSelectImage();   //选择打开图片
	void onPasteImage();    //选择粘贴图片
	void onZoomInImage();   //图片放大
	void onZoomOutImage();  //图片缩小
	void onPresetImage();   //图片还原

private:
	QImage m_image;           //显示的图片
	qreal m_zoomValue = 1.0;  //鼠标缩放值
	int m_xPtInterval = 0;    //平移X轴的值
	int m_yPtInterval = 0;    //平移Y轴的值
	QPoint m_oldPos;          //旧的鼠标位置
	bool m_pressed = false;   //鼠标是否被摁压
	QString m_localFileName;  //文件名称
	QMenu* m_menu;            //右键菜单
};

#endif // PHOTOLABEL_H
