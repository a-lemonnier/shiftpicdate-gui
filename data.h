#ifndef DATA_H
#define DATA_H

#include <QString>

namespace spdData {


}

namespace spdStyle {

    const QString TimeField=
            "border-style: outset;"
            "border-radius: 7px;"
            "border-color: red;"
            "min-height: 27px;"
            "min-width: 25px";

    const QString TimeFieldGreen=
            "border: 1px solid green;"
            "border-style: outset;"
            "border-radius: 7px;"
            "border-color: green;"
            "min-height: 25px;"
            "min-width: 25px";

    const QString bBrowseRed=
            "font: 75 9pt 'DejaVu Sans Condensed'; "
            "color: rgb(255, 0, 0); "
            "border-color: rgb(255, 0, 0);";

    const QString lEPathRed=
            "border: 1px solid red;"
            "border-style: outset;"
            "border-radius: 8px;"
            "border-color: red;"
            "min-height: 25px;"
            "background: qlineargradient("
                "spread:pad, "
                "x1:0.376, y1:0.534091, "
                "x2:1.5, y2:0.696409, "
                "stop:0.0316742 rgba(255, 255, 255, 0), "
                "stop:1 rgba(170, 0, 0, 203));";

    const QString bRunGreen=
            "font: 75 9pt 'DejaVu Sans Condensed'; "
            "color: rgb(0, 200, 0); "
            "border-color: rgb(0, 200, 0);";

    const QString tbLogGreen=
            "QTextBrowser {"
            "background-color: qlineargradient("
                    "spread:pad, "
                    "x1:0.03, y1:1, "
                    "x2:0.10, y2:0.68, "
                    "stop:0 rgba(0, 255, 0, 120), "
                    "stop:0.5 rgba(255, 255, 255, 0));"
            "}"
            "QScrollBar:vertical {"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "    subcontrol-position: bottom;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0 px;"
            "    subcontrol-position: top;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar:horizontal {"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::add-line:horizontal {"
            "    height: 0px;"
            "    subcontrol-position: bottom;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar::sub-line:horizontal {"
            "    height: 0 px;"
            "    subcontrol-position: top;"
            "    subcontrol-origin: margin;"
            "}";

    const QString ScrollBar=
            "QScrollBar:vertical {"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "    subcontrol-position: bottom;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0 px;"
            "    subcontrol-position: top;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar:horizontal {"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::add-line:horizontal {"
            "    height: 0px;"
            "    subcontrol-position: bottom;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar::sub-line:horizontal {"
            "    height: 0 px;"
            "    subcontrol-position: top;"
            "    subcontrol-origin: margin;"
            "}";

    const QString BlinkRadial=
            "background: qradialgradient("
              "spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
              "stop:0 rgba(50, 0, 150, 100), stop:1 rgba(255, 255, 255, 200));";

     const QString Blink=
             "background-color: qlineargradient("
               "spread:pad, x1:0, y1:0, x2:1, y2:1, "
               "stop:0 rgba(0, 0, 100, 100), stop:1 rgba(255, 255, 255, 175));";
}


#endif // DATA_H
