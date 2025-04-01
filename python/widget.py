# This Python file uses the following encoding: utf-8
import sys, re

from PySide6.QtCore import (QFileInfo, QByteArray, QBuffer, QCryptographicHash)

from PySide6.QtGui import (QAction, QImage, QIcon)

from PySide6.QtWidgets import QApplication, QWidget, QMessageBox, QFileDialog

# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_Widget


class Widget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.m_format = self.ui.comboBox.currentText()
        self.ui.aboutButton.clicked.connect(self.on_aboutButton_clicked)
        self.ui.pasteButton.clicked.connect(self.on_pasteButton_clicked)
        self.ui.openButton.clicked.connect(self.on_openButton_clicked)
        self.ui.copyButton.clicked.connect(self.on_copyButton_clicked)
        self.ui.saveButton.clicked.connect(self.on_saveButton_clicked)
        self.ui.imageButton.clicked.connect(self.on_imageButton_clicked)
        self.ui.codeButton.clicked.connect(self.on_codeButton_clicked)
        self.ui.checkBox.checkStateChanged.connect(self.on_checkBox_checkStateChanged)
        self.ui.comboBox.currentTextChanged.connect(self.on_comboBox_currentTextChanged)

    def update_code(self):
        image = self.ui.viewLabel.get_image()
        if image.isNull():
            QMessageBox.warning(self, "Error", "Please load an image file!")
            return
        ba = QByteArray()
        buf = QBuffer(ba)
        image.save(buf, self.m_format)
        md5 = QCryptographicHash.hash(ba, QCryptographicHash.Md5)
        strMd5 = str(md5.toHex())[2:-1]
        prefix = "data:image/{};base64,".format(self.m_format)
        code = str(ba.toBase64())[2:-1]
        if self.ui.checkBox.isChecked():
            self.ui.textEdit.setText("![{0}]({1})".format(strMd5, prefix + code))
        else:
            self.ui.textEdit.setText(prefix + code)
        buf.close()
        num = len(self.ui.textEdit.toPlainText())
        self.ui.lengthLabel.setText("Length : " + str(num))

    def update_image(self):
        p_b = self.ui.textEdit.toPlainText()
        if len(p_b) == 0:
            return
        if re.match("data:image[/a-z]*;base64,", p_b):
            p_b = re.sub("data:image[/a-z]*;base64,", "", p_b, count=1)
        image = QImage()
        if image.loadFromData(QByteArray.fromBase64(p_b.encode())) is False:
            QMessageBox.warning(self, "Error", "Fail to decrypt codes!")
            return
        self.ui.viewLabel.set_image(image)

    def on_pasteButton_clicked(self):
        self.ui.viewLabel.paste_action()

    def on_openButton_clicked(self):
        self.ui.viewLabel.open_action()

    def on_copyButton_clicked(self):
        clipboard = QApplication.clipboard()
        if clipboard is None:
            QMessageBox.warning(self, "Error", "Cannot load system clipboard!")
            return
        text = self.ui.textEdit.toPlainText()
        if len(text) == 0:
            QMessageBox.warning(self, "Error", "No text!")
            return;
        clipboard.setText(text)

    def on_saveButton_clicked(self):
        if self.ui.viewLabel.get_image().isNull() and self.ui.textEdit.toPlainText().isEmpty():
            QMessageBox.warning(self, "Error", "No contents!")
            return;
        strFileName, _ = QFileDialog.getSaveFileName(self, "Save image", "",
                                                     "TXT(*.txt);;JPG(*.jpg);;PNG(*.png);;ICO(*.ico);;BMP(*.bmp)")
        if len(strFileName) > 0:
            subfix = strFileName[-3:]  # 取后缀名
            if subfix.casefold() == "txt".casefold():  # 判断后缀名是否是文本文件
                with open(strFileName, "w") as file:
                    file.write(self.ui.textEdit.toPlainText())  # 自带文件关闭功能，不需要再写f.close()
            else:
                if self.ui.viewLabel.get_image().save(strFileName, subfix) is False:  # 保存图片
                    QMessageBox.warning(self, "Error", "Fail to save image file!")

    def on_checkBox_checkStateChanged(self, arg):
        pass

    def on_comboBox_currentTextChanged(self, arg1: str):
        self.m_format = self.ui.comboBox.currentText()

    def on_codeButton_clicked(self):
        self.update_code()

    def on_imageButton_clicked(self):
        self.update_image()

    def on_aboutButton_clicked(self):
        msgBox = QMessageBox()
        msgBox.setWindowIcon(QIcon(":/img/ico/app.ico"))
        msgBox.setWindowTitle("About...")
        msgBox.setText("    This application is used for mutual conversion between image files and base64 encoding.\n"
                       "    The main usage scenario is Markdown insertion of images, which supports reading images from the clipboard.\n"
                       "    To ensure the efficiency of the program, please do not open image files that are too large to prevent the program from crashing.\n"
                       "\t\t\t\t\t\t\tVersion : v1.0.0")
        msgBox.addButton("&OK", QMessageBox.AcceptRole)
        msgBox.setIcon(QMessageBox.Information)
        msgBox.exec()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())
