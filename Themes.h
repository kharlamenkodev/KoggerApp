#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QFont>
#include <QColor>

class Themes : public QObject
{
    Q_OBJECT
public:

    Themes() {
        setTheme();
        _isConsoleVisible = false;
    }


    Q_PROPERTY(QColor disabledTextColor READ disabledTextColor NOTIFY changed)
    Q_PROPERTY(QColor disabledBackColor READ disabledBackColor NOTIFY changed)
    Q_PROPERTY(QColor hoveredBackColor READ hoveredBackColor NOTIFY changed)

    Q_PROPERTY(QColor textColor READ textColor NOTIFY changed)
    Q_PROPERTY(QColor textSolidColor READ textSolidColor NOTIFY changed)
    Q_PROPERTY(QColor textErrorColor READ textErrorColor NOTIFY changed)
    Q_PROPERTY(QFont textFont READ textFont NOTIFY changed)
    Q_PROPERTY(QFont textFontS READ textFontS NOTIFY changed)

    Q_PROPERTY(QColor menuBackColor READ menuBackColor NOTIFY changed)
    Q_PROPERTY(QColor controlBackColor READ controlBackColor NOTIFY changed)
    Q_PROPERTY(QColor controlBorderColor READ controlBorderColor NOTIFY changed)
    Q_PROPERTY(QColor controlSolidBackColor READ controlSolidBackColor NOTIFY changed)
    Q_PROPERTY(QColor controlSolidBorderColor READ controlSolidBorderColor NOTIFY changed)
    Q_PROPERTY(int controlHeight READ controlHeight NOTIFY changed)
    Q_PROPERTY(int menuWidth READ menuWidth NOTIFY changed)

    Q_PROPERTY(int themeID READ themeId WRITE setTheme NOTIFY changed)

    Q_PROPERTY(bool consoleVisible READ consoleVisible WRITE setConsoleVisible NOTIFY interfaceChanged)

    QColor textColor() { return *_textColor; }
    QColor textErrorColor() { return *_textErrorColor; }
    QColor disabledTextColor() { return *_disabledTextColor; }
    QColor disabledBackColor() {return *_disabledBackColor;}
    QColor hoveredBackColor() { return *_hoveredBackColor; }
    QColor textSolidColor() { return *_textSolidColor; }
    QFont textFont() { return *_textFont; }
    QFont textFontS() { return *_textFontS; }

    QColor menuBackColor() { return *_menuBackColor; }
    QColor controlBackColor() { return *_controlBackColor; }
    QColor controlBorderColor() { return *_controlBorderColor; }
    QColor controlSolidBackColor() { return *_controlSolidBackColor; }
    QColor controlSolidBorderColor() { return *_controlSolidBorderColor; }
    int controlHeight() { return _controlHeight; }
    int menuWidth() { return _menuWidth; }

    void setTheme(int theme_id = 0) {
        _id = theme_id;

#if defined(Q_OS_ANDROID)
        _textFont = new QFont("Times", 30);
        _textFont->setPixelSize(30);
        _textFontS = new QFont("Times", 24);
        _textFontS->setPixelSize(24);
#else
        _textFont = new QFont("PT Sans Caption", 14);
        _textFontS = new QFont("PT Sans Caption", 12);
#endif
        _textErrorColor = new QColor(250, 0, 0);

        if(theme_id == 0) {
            _textColor = new QColor(250, 250, 250);
            _textSolidColor = new QColor(250, 250, 250);
            _menuBackColor = new QColor(45, 45, 45, 240);
            _controlBackColor = new QColor(60, 60, 60);
            _controlBorderColor = new QColor(100, 100, 100);
            _controlSolidBackColor = new QColor(100, 100, 100);
            _controlSolidBorderColor = new QColor(150, 150, 150);

            _disabledTextColor = new QColor(150, 150, 150);
            _disabledBackColor = new QColor(50, 50, 50);
            _hoveredBackColor = new QColor(70,70,70);

        } else if(theme_id == 1) {
            _textColor = new QColor(255, 255, 255);
            _textSolidColor = new QColor(0, 0, 0);
            _menuBackColor = new QColor(0, 0, 0, 255);
            _controlBackColor = new QColor(55, 55, 55);
            _controlBorderColor = new QColor(155, 155, 155);
            _controlSolidBackColor = new QColor(255, 255, 255);
            _controlSolidBorderColor = new QColor(0, 0, 0, 0);

            _disabledTextColor = new QColor(150, 150, 150);
            _disabledBackColor = new QColor(50, 50, 50);

        } else if(theme_id == 2) {
            _textColor = new QColor(25, 25, 25);
            _textSolidColor = new QColor(25, 25, 25);
            _menuBackColor = new QColor(240, 240, 240, 240);
            _controlBackColor = new QColor(250, 250, 250);
            _controlBorderColor = new QColor(100, 100, 100);
            _controlSolidBackColor = new QColor(255, 255, 255);
            _controlSolidBorderColor = new QColor(150, 150, 150);

            _disabledTextColor = new QColor(150, 150, 150);
            _disabledBackColor = new QColor(50, 50, 50);

        } else if(theme_id == 3) {
            _textColor = new QColor(0, 0, 0);
            _textSolidColor = new QColor(255, 255, 255);
            _menuBackColor = new QColor(250, 250, 250, 250);
            _controlBackColor = new QColor(255, 255, 255);
            _controlBorderColor = new QColor(100, 100, 100);
            _controlSolidBackColor = new QColor(0, 0, 0);
            _controlSolidBorderColor = new QColor(255, 255, 255);

            _disabledTextColor = new QColor(150, 150, 150);
            _disabledBackColor = new QColor(50, 50, 50);
        }
#if defined(Q_OS_ANDROID)
        _controlHeight = 48;
#else
        _controlHeight = 26;
#endif
        emit changed();
    }

    int themeId() { return _id; }

    void setConsoleVisible(bool vis) {
        _isConsoleVisible = vis;
        interfaceChanged();
    }

    bool consoleVisible() { return _isConsoleVisible; }

signals:
    void changed();

    void interfaceChanged();
protected:
    int _id = 0;

    QColor* _textColor;
    QColor* _textSolidColor;
    QColor* _textErrorColor;
    QColor* _disabledTextColor;
    QColor* _disabledBackColor;
    QColor* _hoveredBackColor;
    QFont* _textFont;
    QFont* _textFontS;

    QColor* _menuBackColor;
    QColor* _controlBackColor;
    QColor* _controlBorderColor;
    QColor* _controlSolidBackColor;
    QColor* _controlSolidBorderColor;
    int32_t _controlHeight;
    int32_t _menuWidth;

    bool _isConsoleVisible;
};

#endif // THEME_H
