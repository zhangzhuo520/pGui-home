#include "menuwidget.h"

MenuColor::MenuColor(QString objectName)
{
    setObjectName(objectName);

    colorwidget = new ColorWidget(this);
    ActionWidget = new QWidgetAction(this);

    ActionWidget->setDefaultWidget(colorwidget);
    addAction(ActionWidget);
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
    connect(ColorButton, SIGNAL(clicked()), this, SLOT(slot_selectColor()));
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
               << QColor(207, 42, 39)
               << QColor(255, 153, 0)
               << QColor(255, 255, 0)
               << QColor(0, 158, 15)
               << QColor(0, 255, 255)
               << QColor(43, 120, 228)
               << QColor(153, 0, 255)
               << QColor(255, 0, 255)

               << QColor(234, 153, 153)
               << QColor(249, 203, 156)
               << QColor(255, 229, 153)
               << QColor(182, 215, 168)
               << QColor(162, 196, 201)
               << QColor(159, 197, 248)
               << QColor(180, 167, 214)
               << QColor(213, 166, 189)

               << QColor(224, 102, 102)
               << QColor(246, 178, 107)
               << QColor(255, 217, 102)
               << QColor(147, 196, 125)
               << QColor(118, 165, 175)
               << QColor(111, 168, 220)
               << QColor(142, 124, 195)
               << QColor(194, 123, 160)

               << QColor(204,  0 ,  0)
               << QColor(230, 145, 56)
               << QColor(241, 194, 50)
               << QColor(106, 168, 79)
               << QColor(169, 129, 142)
               << QColor(89, 126, 170)
               << QColor(103, 78, 167)
               << QColor(166, 77, 121)

               << QColor(153, 0, 0)
               << QColor(180, 95, 6)
               << QColor(119, 144, 0)
               << QColor(56, 118, 29)
               << QColor(19, 79, 92)
               << QColor(8, 83, 148)
               << QColor(52, 28, 117)
               << QColor(116, 27, 71)

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
        }
        else
        {
              cItem = new ColorItem(this, QColor(245 , 245, 245));
        }
        Hlayout->addWidget(cItem);
        cItemList.append(cItem);
        Hlayout->setSpacing(3);
    }
}

void ColorWidget::slot_selectColor()
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

void ColorItem::slot_selectItemColor()
{

}

MenuStyle::MenuStyle(QString objectName)
{
    setObjectName(objectName);
    stylewidget = new StyleWidget(this);
    ActionWidget = new QWidgetAction(this);
    ActionWidget->setDefaultWidget(stylewidget);
    addAction(ActionWidget);
}

StyleWidget::StyleWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    setMinimumSize(QSize(200,120));
    setMaximumSize(QSize(200,160));
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout();
    initBrushList();
    initStyleHistory();
    initStyleFrame();
    VLayout->addLayout(Hlayout);
    VLayout->addWidget(StyleFrame);
    setLayout(VLayout);
}

void StyleWidget::initBrushList()
{
    BrushList << QBrush(Qt::black, Qt::NoBrush)
              << QBrush(Qt::black, Qt::SolidPattern)
              << QBrush(Qt::black, Qt::Dense1Pattern)
              << QBrush(Qt::black, Qt::Dense2Pattern)
              << QBrush(Qt::black, Qt::Dense3Pattern)
              << QBrush(Qt::black, Qt::Dense4Pattern)
              << QBrush(Qt::black, Qt::Dense5Pattern)
              << QBrush(Qt::black, Qt::Dense6Pattern)
              << QBrush(Qt::black, Qt::Dense7Pattern)
              << QBrush(Qt::black, Qt::HorPattern)
              << QBrush(Qt::black, Qt::VerPattern)
              << QBrush(Qt::black, Qt::CrossPattern)
              << QBrush(Qt::black, Qt::BDiagPattern)
              << QBrush(Qt::black, Qt::FDiagPattern)
              << QBrush(Qt::black, Qt::DiagCrossPattern)
              << QBrush(Qt::black, Qt::NoBrush)
              << QBrush(Qt::black, Qt::NoBrush)
              << QBrush(Qt::black, Qt::NoBrush)
              << QBrush(Qt::black, Qt::NoBrush);
}

void StyleWidget::initStyleHistory()
{
    for (int i = 0; i < 9; i ++)
    {
        if(i < UiStyle::ItemBrushList.count())
        {
              Item = new StyleItem(this, UiStyle::ItemBrushList.at(i));
        }
        else
        {
              Item = new StyleItem(this, QBrush(QColor(245, 245, 245), Qt::NoBrush));
        }
        Hlayout->addWidget(Item);
    }
    Hlayout->setSpacing(5);
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
    for(int i = 0; i < BrushList.count(); i ++)
    {
        Item = new StyleItem(this, BrushList.at(i));
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
    if (HLayoutNum > BrushList.count() / 5)
    {
        QSpacerItem *spaceItem = new QSpacerItem(200, 15, QSizePolicy::Preferred, QSizePolicy::Preferred);
        HLayout->addSpacerItem(spaceItem);
        HLayout->setStretch(0, 1);
        HLayout->setStretch(1, 6);
    }
    StyleFrame->setLayout(VLayoutFrame);
}

void StyleItem::slot_selectItemStyle()
{
}



