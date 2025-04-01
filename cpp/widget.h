#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
	class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = nullptr);
	~Widget();

private slots:
	void on_pasteButton_clicked();
	void on_openButton_clicked();
	void on_copyButton_clicked();
	void on_saveButton_clicked();
	void updateCode();
	void on_checkBox_checkStateChanged(const Qt::CheckState& arg1);
	void on_comboBox_currentTextChanged(const QString& arg1);
	void on_codeButton_clicked();
	void on_imageButton_clicked();
	void updateImage();
	void on_aboutButton_clicked();

private:
	Ui::Widget* ui;
	QString format;
};
#endif // WIDGET_H
