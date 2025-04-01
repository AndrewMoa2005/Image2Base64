#include "widget.h"
#include "ui_widget.h"
#include "version.h"
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include "photolabel.h"
#include <QCryptographicHash>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QRegularExpression>

Widget::Widget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::Widget)
{
	ui->setupUi(this);

	format = ui->comboBox->currentText();
	/*
		QObject::connect(ui->viewer, &PhotoLabel::imageLoadSuccess,
			this, &Widget::updateCode);
	*/
}

Widget::~Widget()
{
	delete ui;
}

void Widget::on_pasteButton_clicked()
{
	ui->viewer->pasteAction();
}


void Widget::on_openButton_clicked()
{
	ui->viewer->openAction();
}


void Widget::on_copyButton_clicked()
{
	QClipboard* clipboard = QApplication::clipboard();

	if (!clipboard)
	{
		QMessageBox::warning(this, tr("Error"), tr("Cannot load system clipboard!"));
		return;
	}

	QString text = ui->textEdit->toPlainText();
	if (text.isEmpty())
	{
		QMessageBox::warning(this, tr("Error"), tr("No text!"));
		return;
	}
	clipboard->setText(text);
}


void Widget::on_saveButton_clicked()
{
	if (ui->viewer->getImage().isNull() && ui->textEdit->toPlainText().isEmpty())
	{
		QMessageBox::warning(this, tr("Error"), tr("No contents!"));
		return;
	}

	QString strFileName = QFileDialog::getSaveFileName(this, tr("Save image"), "", "TXT(*.txt);;JPG(*.jpg);;PNG(*.png);;ICO(*.ico);;BMP(*.bmp)");
	if (!strFileName.isNull())
	{
		int nIndex = strFileName.lastIndexOf('.');  //寻找‘.’符号在字符串中的id
		nIndex++;
		int nLen = strFileName.length() - nIndex;
		QString strSuffix = strFileName.right(nLen);    //截取‘.’符号后面的字符串，这是为了获取用户选择的文件后缀名
		if (strSuffix.contains("TXT", Qt::CaseInsensitive))
		{
			//内容保存到路径文件
			QFile file(strFileName);
			//以文本方式打开
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream out(&file);     //IO设备对象的地址对其进行初始化
				out << ui->textEdit->toPlainText() << Qt::endl;//输出
				file.close();
			}
			else
			{
				QMessageBox::warning(this, tr("Error"), tr("Fail to save text file!"));
			}
		}
		else {
			if (!ui->viewer->getImage().save(strFileName, strSuffix.toLower().toUtf8()))  //保存图片
			{
				QMessageBox::warning(this, tr("Error"), tr("Fail to save image file!"));
			}
		}
	}
}

void Widget::updateCode()
{
	QImage image = ui->viewer->getImage();
	if (image.isNull())
	{
		QMessageBox::warning(this, tr("Error"), tr("Please load an image file!"));
		return;
	}
	QByteArray ba;
	QBuffer buf(&ba);
	image.save(&buf, format.toStdString().c_str());

	QByteArray md5 = QCryptographicHash::hash(ba, QCryptographicHash::Md5);
	QString strMd5 = md5.toHex();

	QString head_md = QString::fromUtf8("![%1](%2)");
	QString prefix = QString::fromUtf8("data:image/%1;base64,").arg(format);
	QString code = QString::fromUtf8(ba.toBase64());

	if (ui->checkBox->isChecked())
	{
		ui->textEdit->setText(head_md.arg(strMd5).arg(prefix + code));
	}
	else
	{
		ui->textEdit->setText(prefix + code);
	}

	buf.close();

	int num = ui->textEdit->toPlainText().length();
	ui->label->setText(tr("Length : ") + QString::number(num));
}

void Widget::on_checkBox_checkStateChanged(const Qt::CheckState& arg1)
{
	//updateCode();
}


void Widget::on_comboBox_currentTextChanged(const QString& arg1)
{
	format = ui->comboBox->currentText();
	//updateCode();
}


void Widget::on_codeButton_clicked()
{
	updateCode();
}


void Widget::on_imageButton_clicked()
{
	updateImage();
}

void Widget::updateImage()
{
	QString p_b = ui->textEdit->toPlainText();
	if (p_b.isEmpty())
	{
		return;
	}
	if (p_b.contains(QRegularExpression("data:image[/a-z]*;base64,")))
	{
		p_b = p_b.remove(QRegularExpression("data:image[/a-z]*;base64,"));
	}

	QImage image;
	if (!image.loadFromData(QByteArray::fromBase64(p_b.toLocal8Bit())))
	{
		QMessageBox::warning(this, tr("Error"), tr("Fail to decrypt codes!"));
		return;
	}
	ui->viewer->setImage(image);
}

void Widget::on_aboutButton_clicked()
{
	QMessageBox msgBox;
	msgBox.setWindowIcon(QIcon(":/img/ico/app.ico"));
	msgBox.setWindowTitle(tr("About..."));
	QString text = QString(tr("    This application is used for mutual conversion between image files and base64 encoding.\n"
		"    The main usage scenario is Markdown insertion of images, which supports reading images from the clipboard.\n"
		"    To ensure the efficiency of the program, please do not open image files that are too large to prevent the program from crashing.\n"
		"\t\t\t\t\t\t\tVersion : "
		PROJECT_VERSION));
	msgBox.setText(text);
	msgBox.addButton(tr("&OK"), QMessageBox::AcceptRole);
	msgBox.setIcon(QMessageBox::Information);
	msgBox.exec();
}
