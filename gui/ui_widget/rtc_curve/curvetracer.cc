#include "curvetracer.h"

CurveTracer::CurveTracer(QCustomPlot *_plot, QCPGraph *_graph, TracerType _type) : plot(_plot),
    graph(_graph),
    type(_type),
    visible(false)

{
    if (plot)
    {
        tracer = new QCPItemTracer(plot);
        tracer->setStyle(QCPItemTracer::tsPlus);//可以选择设置追踪光标的样式，这个是小十字，还有大十字，圆点等样式
//        tracer->setPen(QPen(Qt::red));
        tracer->setPen(graph->pen());//设置tracer的颜色
        tracer->setBrush(graph->pen().color());
        tracer->setSize(10);

        label = new QCPItemText(plot);
        label->setLayer("overlay");
        label->setClipToAxisRect(false);
        label->setPadding(QMargins(5, 5, 5, 5));

        label->position->setParentAnchor(tracer->position);
        label->setFont(QFont("宋体", 10));

        arrow = new QCPItemLine(plot);
        arrow->setLayer("overlay");
        arrow->setPen(graph->pen());//设置箭头的颜色
        arrow->setClipToAxisRect(false);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        switch (type) {
        case XAxisTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));

            label->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(20, 0);//偏移量
            break;
        }
        case YAxisTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));
            label->setPositionAlignment(Qt::AlignRight|Qt::AlignHCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(label->position);
            arrow->start->setCoords(-20, 0);//偏移量
            break;
        }
        case DataTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(244, 244, 244, 150)));
            label->setPen(graph->pen());
            label->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(25, 0);
            break;
        }

        default:
            break;
        }

        setVisible(false);
    }
}

CurveTracer::~CurveTracer()
{
    if (tracer)
        plot->removeItem(tracer);
    if (label)
        plot->removeItem(label);
}

void CurveTracer::setPen(const QPen &pen)
{
    tracer->setPen(pen);
    arrow->setPen(pen);
}

void CurveTracer::setBrush(const QBrush &brush)
{
    tracer->setBrush(brush);
}

void CurveTracer::setLabelPen(const QPen &pen)
{
    label->setPen(pen);
}

void CurveTracer::setText(const QString &text)
{
    label->setText(text);
}

void CurveTracer::setVisible(bool visible)
{
    tracer->setVisible(visible);
    label->setVisible(visible);
    arrow->setVisible(visible);
}

void CurveTracer::updatePosition(double xValue, double yValue)
{
//    if (mGraph)
//    {
//        if (mParentPlot->hasPlottable(mGraph))
//        {
//            if (mGraph->data()->size() > 1)
//            {
//                QCPGraphDataContainer::const_iterator first = mGraph->data()->constBegin();
//                QCPGraphDataContainer::const_iterator last = mGraph->data()->constEnd()-1;
//                if (mGraphKey <= first->key)//判断当前的key是否小于第一个点的key
//                {
//                    position->setCoords(first->key, first->value);
//                    }
//                else if (mGraphKey >= last->key)
//                    position->setCoords(last->key, last->value);
//                else
//                {
//                    QCPGraphDataContainer::const_iterator it = mGraph->data()->findBegin(mGraphKey);
//                    //敲重点！ 这里是找key对应value的关键实现 调用containner的函数找到key对应的数据索引，没看错 他就是一个一个找过去的，没有使用任何算法实现，当数据量很大时弊端就出现了，鼠标每次移动都需要从头开始重新遍历寻找
//                    if (it != mGraph->data()->constEnd()) // mGraphKey is not exactly on last iterator, but somewhere between iterators
//                    {
//                        QCPGraphDataContainer::const_iterator prevIt = it;
//                        ++it; // won't advance to constEnd because we handled that case (mGraphKey >= last->key) before
//                        if (mInterpolating)
//                        {              // interpolate between iterators around mGraphKey:
//                            double slope = 0;
//                            if (!qFuzzyCompare((double)it->key, (double)prevIt->key))
//                                slope = (it->value-prevIt->value)/(it->key-prevIt->key);
//                            position->setCoords(mGraphKey, (mGraphKey-prevIt->key)*slope+prevIt->value);
//                        } else
//                        {              // find iterator with key closest to mGraphKey:
//                            if (mGraphKey < (prevIt->key+it->key)*0.5)
//                                position->setCoords(prevIt->key, prevIt->value);
//                            else
//                                position->setCoords(it->key, it->value);
//                        }
//                    } else // mGraphKey is exactly on last iterator (should actually be caught when comparing first/last keys, but this is a failsafe for fp uncertainty)
//                        position->setCoords(it->key, it->value);
//                }      } else if (mGraph->data()->size() == 1)
//            {
//                QCPGraphDataContainer::const_iterator it = mGraph->data()->constBegin();
//                position->setCoords(it->key, it->value);
//            } else
//                qDebug() << Q_FUNC_INFO << "graph has no data";
//        } else
//            qDebug() << Q_FUNC_INFO << "graph not contained in QCustomPlot instance (anymore)";
//    }

}
