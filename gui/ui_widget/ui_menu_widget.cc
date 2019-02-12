#include "ui_menu_widget.h"
namespace ui {
MenuColor::MenuColor(QString objectName)
{
    setObjectName(objectName);

    colorwidget = new ColorWidget(this);
    ActionWidget = new QWidgetAction(this);
    ActionWidget->setDefaultWidget(colorwidget);
    addAction(ActionWidget);
    connect(colorwidget, SIGNAL(signal_selectColor(QColor)), this, SLOT(slot_seletColor(QColor)));
}

void MenuColor::slot_seletColor(QColor color)
{
    emit signal_selectColor(color);
}

ColorWidget::ColorWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    setMinimumSize(QSize(200,180));
    setMaximumSize(QSize(200,180));
    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    VlayoutWidget = new QWidget(this);
    VlayoutWidget->setGeometry(0,2,200,35);

    initColorList();
    initColorHistory();
    initColorFrame();
    ColorFrame->setGeometry(0, 38, 200, 140);
    VlayoutWidget->setLayout(Hlayout);
    connect(ColorButton, SIGNAL(clicked()), this, SLOT(slot_colorDialogSelect()));
}

void ColorWidget::initColorList()
{    ColorList << QColor(0, 0, 0)
               << QColor(51, 51, 51)
               << QColor(102, 102, 102)
               << QColor(153, 153, 153)
               << QColor(204, 204, 204)
               << QColor(221, 221, 221)
               << QColor(238, 238, 238)
               << QColor(255, 255, 255)

               << QColor(139, 0, 0)
               << QColor(154, 205, 50)
               << QColor(175, 238, 238)
               << QColor(0, 191, 255)
               << QColor(139, 69, 19)
               << QColor(102, 205,170)
               << QColor(123, 104,238)
               << QColor(0,0,205)


               << QColor(85,107,47)
               << QColor(100,149,237)
               << QColor(218,112,214)
               << QColor(148,0,211)
               << QColor(107, 142, 35)
               << QColor(147, 112, 219)
               << QColor(220,20,60)
               << QColor(255,0,0)

               << QColor(32, 178,170)
               << QColor(238,130,238)
               << QColor(255, 105,180)
               << QColor(255,0,255)
               << QColor(70,130,180)
               << QColor(255,182,193)
               << QColor(255, 140, 0)
               << QColor(0,0,255)
               << QColor(139, 0, 139)
               << QColor(230, 145, 56)
               << QColor(255, 215, 0)
               << QColor(0, 128, 0)
               << QColor(79, 80, 81)
               << QColor(192,197,204)
               << QColor(210, 105, 30)
               << QColor(240, 230, 140)

               << QColor(50,205,50)
               << QColor(0, 255, 0)
               << QColor(218, 165, 32)
               << QColor(127, 255, 212)
               << QColor(0, 128, 128)
               << QColor(8, 83, 148)
               << QColor(52, 28, 117)
               << QColor(255, 255, 0)

               << QColor(102, 0, 0)
               << QColor(120, 63, 4)
               << QColor(127, 96, 0)
               << QColor(39, 78, 19)
               << QColor(12, 52, 61)
               << QColor(7, 55, 99)
               << QColor(32, 18, 77)
               << QColor(71, 17, 48);
}

void ColorWidget::initColorFrame()
{
    ColorFrame = new QFrame(this);
    ColorFrame->setAutoFillBackground(true);
    QPalette pale = ColorFrame->palette();
    pale.setColor(QPalette::Window, QColor(180, 180, 180));
    ColorFrame->setPalette(pale);
    QVBoxLayout *VLayout = new QVBoxLayout();
    QHBoxLayout *HLayout = NULL;
    ColorButton = new PushButton(this);
    ColorButton->setIcon(QIcon(":/dfjy/images/colorIcon.png"));


    for (int j = 0; j < 7; j ++)
    {
        HLayout = new QHBoxLayout();
        if (j == 6)
        {
            for (int i = 0; i < 2; i ++)
            {
                cItem = new ColorItem(this, ColorList.at(j * 9 + i));
                connect(cItem, SIGNAL(signal_select_color(QColor)), this, SLOT(slot_selectColor(QColor)));
                HLayout->addWidget(cItem);
            }
            QSpacerItem *spaceItem = new QSpacerItem(100, 10, QSizePolicy::Preferred, QSizePolicy::Preferred);
            HLayout->addSpacerItem(spaceItem);
            HLayout->addWidget(ColorButton);
            HLayout->setSpacing(3);
            HLayout->setMargin(0);
            HLayout->setStretch(0, 1);
            HLayout->setStretch(1, 1);
            HLayout->setStretch(2, 7);
            HLayout->setStretch(3, 1);
        }
        else
        {
            for (int i = 0; i < 9; i ++)
            {
                cItem = new ColorItem(this, ColorList.at(j * 9 + i));
                connect(cItem, SIGNAL(signal_select_color(QColor)), this, SLOT(slot_selectColor(QColor)));
                HLayout->addWidget(cItem);
                HLayout->setSpacing(3);
            }
        }

        HLayout->setMargin(1);
        VLayout->addLayout(HLayout);
        VLayout->setSpacing(3);
    }
    ColorFrame->setLayout(VLayout);
}

void ColorWidget::initColorHistory()
{
    for (int i = 0; i < 9; i ++)
    {
        if(i < UiStyle::ItemColorList.count())
        {
            cItem = new ColorItem(this, UiStyle::ItemColorList.at(i));
            connect(cItem, SIGNAL(signal_select_color(QColor)), this, SLOT(slot_selectColor(QColor)));
        }
        else
        {
            cItem = new ColorItem(this, QColor(245 , 245, 245));
            connect(cItem, SIGNAL(signal_select_color(QColor)), this, SLOT(slot_selectColor(QColor)));
        }
        Hlayout->addWidget(cItem);
        cItemList.append(cItem);
        Hlayout->setSpacing(3);
    }
}

void ColorWidget::slot_selectColor(QColor color)
{
    emit signal_selectColor(color);
}

void ColorWidget::slot_colorDialogSelect()
{
    QColor color = QColorDialog::getColor ();
     static int i = 0;
    if (color.isValid ())
    {
        UiStyle::ItemColorList.append(color);
        if(i > 8) i = i - 9;
        cItemList.at(i ++)->setColor(color);
    }
}

MenuStyle::MenuStyle(QString objectName)
{
    setObjectName(objectName);
    stylewidget = new StyleWidget(this);
    ActionWidget = new QWidgetAction(this);
    ActionWidget->setDefaultWidget(stylewidget);
    addAction(ActionWidget);
    connect(stylewidget, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_selectItemStyle(int)));
}

void MenuStyle::slot_selectItemStyle(int item)
{
    emit signal_selectItemStyle(item);
}

StyleWidget::StyleWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    setMinimumSize(QSize(200,200));
    setMaximumSize(QSize(200,200));
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    initPatternList();
    initStyleHistory();
    initStyleFrame();
    VLayout->addLayout(Hlayout);
    VLayout->addWidget(StyleFrame);
    setLayout(VLayout);
}

void StyleWidget::initPatternList()
{
    for (uint i = 0; i < pattern.count(); i ++)
    {
        PatternList << i;
    }
    UiStyle::ItemPetternList << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
}

void StyleWidget::initStyleHistory()
{
//    for (int i = 0; i < 6; i ++)
//    {
//        if(i < UiStyle::ItemPetternList.count())
//        {
//            Item = new StyleItem(this, UiStyle::ItemPetternList.at(i));
//            connect(Item, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_selectItemStyle(int)));
//        }
//        else
//        {
//            Item = new StyleItem(this, UiStyle::ItemPetternList.at(0));
//            connect(Item, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_selectItemStyle(int)));
//        }
//        Hlayout->addWidget(Item);
//    }
//    Hlayout->setSpacing(5);
}

void StyleWidget::initStyleFrame()
{
    QVBoxLayout *VLayoutFrame = new QVBoxLayout();
    QHBoxLayout *HLayout = NULL;
    StyleFrame = new QFrame(this);
    StyleFrame->setAutoFillBackground(true);
    QPalette pale = StyleFrame->palette();
    pale.setColor(QPalette::Window, QColor(180, 180, 180));
    StyleFrame->setPalette(pale);
    int HLayoutNum = 0;
    for(int i = 0; i < PatternList.count(); i ++)
    {
        Item = new StyleItem(this, PatternList.at(i));
        connect(Item, SIGNAL(signal_selectItemStyle(int)), this, SLOT(slot_selectItemStyle(int)));
        if (i % 6)
        {
            HLayout->addWidget(Item);
        }
        else
        {
            HLayout = new QHBoxLayout();
            HLayout->addWidget(Item);
            HLayoutNum++;
            HLayout->setSpacing(5);
            VLayoutFrame->addLayout(HLayout);
        }
    }
    if (HLayoutNum > PatternList.count() / 5)
    {
        QSpacerItem *spaceItem = new QSpacerItem(200, 15, QSizePolicy::Preferred, QSizePolicy::Preferred);
        HLayout->addSpacerItem(spaceItem);
        HLayout->setStretch(0, 1);
        HLayout->setStretch(1, 6);
    }
    StyleFrame->setLayout(VLayoutFrame);
}

void StyleWidget::slot_selectItemStyle(int item)
{
    emit signal_selectItemStyle(item);
}

QImage &StyleItem::image_color_change(QImage & image, QColor color)
{
    for (int j = 0; j < image.height(); j ++)
    {
        for(int i= 0; i < image.width(); i ++)
        {
            QColor temp_color = uint_to_color(image.pixel(i, j));
            if (temp_color.red() || temp_color.blue() || temp_color.green())
            {
                image.setPixel(i, j , qRgb(color.red(), color.green(), color.blue()));
            }
        }
    }
    return image;
}

QColor StyleItem::uint_to_color(uint color)
{
    int b = color << 24 >> 24;
    int g = color << 16 >> 24;
    int r = color << 8 >> 24;
    return QColor(r, g, b);
}

}


