#!/bin/bash

pyside6-uic form.ui -o ui_form.py
pyside6-rcc appicon.qrc -o appicon_rc.py
pyinstaller --onefile --windowed --name Image2Base64.elf widget.py
