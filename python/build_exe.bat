pyside6-uic form.ui -o ui_form.py
pyside6-rcc appicon.qrc -o appicon_rc.py
pyinstaller --onefile --windowed ^
    --icon "ico/app.ico" --name Image2Base64.exe ^
    widget.py