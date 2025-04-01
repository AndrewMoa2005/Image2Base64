# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtCore import (Qt, qDebug, QFileInfo, QMimeData, QRect, QPoint)

from PySide6.QtGui import (QAction, QImage, QAction, QDragEnterEvent, QDragMoveEvent,
                           QDropEvent, QContextMenuEvent, QPaintEvent, QMouseEvent,
                           QWheelEvent, QPainter, QClipboard, QCursor)

from PySide6.QtWidgets import (QApplication, QLabel, QMenu, QMessageBox, QFileDialog)


class PhotoLabel(QLabel):
    def __init__(self, parent):
        super().__init__(parent)
        self.m_image = QImage()  # 显示的图片
        self.m_zoomValue = 1.0  # 鼠标缩放值
        self.m_xPtInterval = 0  # 平移X轴的值
        self.m_yPtInterval = 0  # 平移Y轴的值
        self.m_oldPos = QPoint()  # 旧的鼠标位置
        self.m_pressed = False  # 鼠标是否被摁压
        self.m_localFileName = None  # 文件名称
        self.m_menu = None
        self.initial_widget()

    def initial_widget(self):
        self.m_menu = QMenu(self)
        load_image = QAction(u"&Open new...", self)
        load_image.triggered.connect(self.on_select_image)
        self.m_menu.addAction(load_image)
        paste_image = QAction(u"&Paste image", self)
        paste_image.triggered.connect(self.on_paste_image)
        self.m_menu.addAction(paste_image)
        self.m_menu.addSeparator()
        zoom_in_action = QAction(u"Zoom in &+", self)
        zoom_in_action.triggered.connect(self.on_zoom_in_image)
        self.m_menu.addAction(zoom_in_action)
        zoom_out_action = QAction(u"Zoom out &-", self)
        zoom_out_action.triggered.connect(self.on_zoom_out_image)
        self.m_menu.addAction(zoom_out_action)
        self.m_menu.addSeparator()
        preset_action = QAction(u"&Reset", self)
        preset_action.triggered.connect(self.on_preset_image)
        self.m_menu.addAction(preset_action)
        self.m_menu.addSeparator()
        # clear_action = QAction(u"&Reset", self)
        # clear_action.triggered.connect(self.clear_show)
        # self.m_menu.addAction(clear_action)

    def open_file(self, path: str):  # 打开图片文件
        if path is None:
            return
        if self.m_image.load(path) is False:
            QMessageBox.warning(self, "Error", "Cannot load file!")
            return
        self.m_zoomValue = 1.0
        self.m_xPtInterval = 0
        self.m_yPtInterval = 0
        self.m_localFileName = path
        self.update()

    def clear_show(self):  # 清空显示
        self.m_localFileName = None
        self.m_image = QImage()
        self.clear()

    def set_image(self, image: QImage):  # 设置图片
        if image is None:
            return
        self.m_zoomValue = 1.0
        self.m_xPtInterval = 0
        self.m_yPtInterval = 0
        self.m_localFileName = None
        self.m_image = image.copy(0, 0, image.width(), image.height())
        self.update()

    def open_action(self):  # 调用打开文件对话框
        self.on_select_image()

    def paste_action(self):  # 粘贴来自剪贴板的图片
        self.on_paste_image()

    def get_image(self) -> QImage:  # 调用存储的图片数据
        return self.m_image

    def contextMenuEvent(self, event: QContextMenuEvent):  # 右键菜单
        pos = event.pos()
        pos = self.mapToGlobal(pos)
        self.m_menu.exec(pos)

    def paintEvent(self, event: QPaintEvent):  # QPaint画图
        if self.m_image.isNull():
            # super().paintEvent(event)
            return
        painter = QPainter(self)
        # 根据窗口计算应该显示的图片的大小
        width = min(self.m_image.width(), self.width())
        height = int(width * 1.0 / (self.m_image.width() * 1.0 / self.m_image.height()))
        height = min(height, self.height())
        width = int(height * 1.0 * (self.m_image.width() * 1.0 / self.m_image.height()))
        # 平移
        painter.translate(self.width() / 2 + self.m_xPtInterval, self.height() / 2 + self.m_yPtInterval)
        # 缩放
        painter.scale(self.m_zoomValue, self.m_zoomValue)
        # 绘制图像
        pic_rect = QRect(int(-width / 2), int(-height / 2), width, height)
        painter.drawImage(pic_rect, self.m_image)
        # super().paintEvent(event)

    def wheelEvent(self, event: QWheelEvent):  # 鼠标滚轮滚动
        value = int(event.angleDelta().y() / 15)
        if value < 0:  # 放大
            self.on_zoom_in_image()
        else:  # 缩小
            self.on_zoom_out_image()
        self.update()

    def mousePressEvent(self, event: QMouseEvent):  # 鼠标摁下
        self.m_oldPos = event.pos()
        self.m_pressed = True
        self.setCursor(Qt.ClosedHandCursor)  # 设置鼠标样式

    def mouseMoveEvent(self, event: QMouseEvent):  # 鼠标松开
        if self.m_pressed is False:
            # super().mouseMoveEvent(event)
            return
        pos = event.pos()
        xp = pos.x() - self.m_oldPos.x()
        yp = pos.y() - self.m_oldPos.y()
        self.m_xPtInterval += xp
        self.m_yPtInterval += yp
        self.m_oldPos = pos
        self.update()

    def mouseReleaseEvent(self, event: QMouseEvent):  # 鼠标发射事件
        self.m_pressed = False
        self.setCursor(Qt.ArrowCursor)  # 设置鼠标样式

    # 拖放文件
    def dragEnterEvent(self, event: QDragEnterEvent):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
        else:
            event.ignore()

    def dragMoveEvent(self, event: QDragMoveEvent):
        pass

    def dropEvent(self, event: QDropEvent):
        urls = event.mimeData().urls()
        if urls is None:
            return
        file_name = urls[0].toLocalFile()
        info = QFileInfo(file_name)
        if info.isFile() is False:
            return
        self.open_file(file_name)

    def on_select_image(self):  # 选择打开图片
        path, _ = QFileDialog.getOpenFileName(self,
                                              "Open a image file", "./",
                                              "Images (*.bmp *.png *.jpg *.jpeg *.gif *.tiff)\nAll files (*.*)")
        if path is None:
            return
        info = QFileInfo(path)
        if info.isFile() is False:
            return
        self.open_file(path)

    def on_paste_image(self):  # 选择粘贴图片
        clipboard = QApplication.clipboard()
        img = clipboard.image()
        if img.isNull():
            return
        self.set_image(img)

    def on_zoom_in_image(self):  # 图片放大
        self.m_zoomValue += 0.05
        self.update()

    def on_zoom_out_image(self):  # 图片缩小
        self.m_zoomValue -= 0.05
        if self.m_zoomValue <= 0:
            self.m_zoomValue = 0.05
            return
        self.update()

    def on_preset_image(self):  # 图片还原
        self.m_zoomValue = 1.0
        self.m_xPtInterval = 0
        self.m_yPtInterval = 0
        self.update()


if __name__ == "__main__":
    pass
