#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qtimer.h>
#include <QDebug>
#include <QMessageBox>
#include <QMap>
#include <qdatetime.h>

#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_plot_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_marker.h>
#include <qwt_date_scale_draw.h>
#include <qwt_abstract_scale_draw.h>

#include <iostream>
#include <fstream>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <mysql/mysql.h>

#include <string>

class YourScaleDraw: public QwtScaleDraw     //吴禹辰写  QwtDateScaleDraw 继承自 QwtScaleDraw, QwtScaleDraw 继承自 QwtAbstractScaleDraw
{
public:
    YourScaleDraw(Qt::Orientation orientation, const QStringList &labels):
        d_labels(labels)
    {
        setTickLength( QwtScaleDiv::MinorTick, 0 );
        setTickLength( QwtScaleDiv::MediumTick, 0 );
        setTickLength( QwtScaleDiv::MajorTick, 2 );

        enableComponent( QwtScaleDraw::Backbone, false );

        if ( orientation == Qt::Vertical ){
            setLabelRotation(-60.0);
        }
        else {
            setLabelRotation(-20.0);
        }
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    }

    virtual QwtText label( double value ) const
    {
        QwtText d_qwttext;

        const int index = qRound( value );
        if( index >= 0 && index < d_labels.size() ){
            d_qwttext = d_labels[ index ];
        }
        return d_qwttext;
    }
private:
    const QStringList d_labels;
};

class TimeScakeDrawR1: public QwtScaleDraw  //QwtDateScaleDraw 继承自 QwtScaleDraw, QwtScaleDraw 继承自 QwtAbstractScaleDraw
{
public:
    TimeScakeDrawR1(const QDateTime &base):
      baseTime(base){
//        setTickLength( QwtScaleDiv::MinorTick, 0 );  //令两个数标之间无刻度
//        setTickLength( QwtScaleDiv::MediumTick, 0 );
//        setTickLength( QwtScaleDiv::MajorTick, 2 );
    }
    virtual QwtText label(double v) const
    {
      QDateTime upTime = baseTime.addSecs((int )v);
//    return upTime.toString("yyyy-MM-dd\nhh:mm:ss");
      return upTime.toString("hh:mm:ss");
    }
private:
    QDateTime baseTime;
};

class PlotPickerByTime: public QwtPlotPicker    //令 picker 显示日期
{
public:
    PlotPickerByTime(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget *widget):
        QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode, widget){}

     QDateTime toDateTime(double value)const
     {
         QDateTime dt = QwtDate::toDateTime( value, Qt::LocalTime );  //如果希望picker显示时间在此处修改,可能需要传入自己设定的时间
         if ( !dt.isValid() ){
             const QDate date = ( value <= 0.0 ) ? QwtDate::minDate() : QwtDate::maxDate();
             dt = QDateTime( date, QTime( 0, 0 ), Qt::LocalTime );
         }
         return dt;
     }

protected:
    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QString text;
        QDateTime time = toDateTime( double(pos.x()) );
        switch (rubberBand()) {    // 认为与选择的表示方式有关,比如使用交叉线或者使用横线,可以参见博客: https://blog.csdn.net/czyt1988/article/details/51555020
        case HLineRubberBand:
            text.sprintf( "%.4f", pos.y() );    //此处笔者没有使用，没有修改，建议根据自己需求进行修改
            break;
        case VLineRubberBand:
            text.sprintf( "%.4f", pos.x() );    //此处笔者没有使用，没有修改，建议根据自己需求进行修改
            break;
        default:
            //text.sprintf( "%.4f, %.4f", pos.x(), pos.y() );    //原代码
            text.append(QString::number(pos.y(),'f',2));         //新增代码
            text.append('('+time.toString("yyyy-MM-dd HH:mm:ss:zzz")+')');    //新增代码,将按照“年-月-日 时：分：秒：毫秒”的格式显示QDateTime默认不显示毫秒
            break;
        }
        return QwtText( text );
    }
};

class QwtPlotBarChart1 : public QwtPlotBarChart
{
public:
    QwtPlotBarChart1():
        QwtPlotBarChart("Page Hits")
    {
        setLegendMode( QwtPlotBarChart::LegendBarTitles );
        setLegendIconSize( QSize( 10,14 ) );
        setLayoutPolicy( AutoAdjustSamples );
        setLayoutHint( 4.0 );  //minimum width for a single bar

        setSpacing( 10 ); //spaceing between bars
    }

    void addbarchart( const QString &distor, const QColor &color )
    {
        d_colors += color;
        d_distros += distor;
        itemChanged();
    }

    virtual QwtColumnSymbol *specialSymbol( int index, const QPointF& ) const
    {
        //we want to have individual colors for each bar

        QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );

        QColor c(Qt::white);
        if(index >= 0 && index < d_colors.size())
            c = d_colors[index];
        symbol->setPalette( c );

        return symbol;
    }

    virtual QwtText barTitle( int sampleIndex ) const
    {
        QwtText title;
        if( sampleIndex >= 0 && sampleIndex < d_distros.size() )
            title = d_distros[sampleIndex];

        return title;
    }

private:
    QList<QColor> d_colors;
    QList<QString> d_distros;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /**********************************************  绘制 qwtplot1 (折线图 散点图)  ***************************************/
    //设置标题
    ui->qwtPlot->detachItems();
    QwtText title;
    title.setText("速度");
    QFont font1("宋体",10);
    title.setFont(font1);
    ui->qwtPlot->setTitle(title);
    ui->qwtPlot->setPalette( Qt::red );
    ui->qwtPlot->setStyleSheet( "background-color:rgb(255, 255, 0, 255);color:rgb(255, 0, 255, 255)" );  //设置背景颜色和前景色  也可以写成background-color:yellow

    //设置画布
    QwtPlotCanvas *canvas;
    canvas = new QwtPlotCanvas();
    canvas->setPalette(Qt::black);
    canvas->setFrameStyle(QFrame::NoFrame);
    ui->qwtPlot->setCanvas(canvas);

    //设置网格
    QwtPlotGrid *grid;
    grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(Qt::green, 0, Qt::DotLine);
    grid->attach(ui->qwtPlot);

    //坐标轴上的字体
    QFont font2("宋体", 12);
    ui->qwtPlot->setAxisFont(QwtPlot::xBottom, font2);
    ui->qwtPlot->setAxisFont(QwtPlot::yLeft, font2);

//    ui->qwtPlot->setAxisAutoScale(QwtPlot::yLeft, true);  //设置自动坐标轴尺度
//    ui->qwtPlot->setAxisAutoScale(QwtPlot::xBottom, true);
    //以下坐标轴刻度的选择不能与上边自动坐标轴共存,只能设置一个,后设置的会覆盖前边的设置
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 5, 1);  //设置坐标轴刻度范围,第一个数字:最小值;第二个数字:最大值;第三个数字:轴标步进
    ui->qwtPlot->setAxisMaxMajor( QwtPlot::xBottom, 5 );  //设置坐标轴有几个数标
    ui->qwtPlot->setAxisMaxMinor( QwtPlot::xBottom, 5 ); //设置坐标轴两个数标的中间分几个小份
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 5, 1);  //设置坐标轴刻度范围,第一个数字:最小值;第二个数字:最大值;第三个数字:轴标步进
    ui->qwtPlot->setAxisMaxMajor( QwtPlot::yLeft, 5 );  //设置坐标轴有几个数标
    ui->qwtPlot->setAxisMaxMinor( QwtPlot::yLeft, 2 ); //设置坐标轴两个数标的中间分几个小份
    //重绘 x轴  实现一:
    QStringList d_distros;
    const Qt::Orientation orientation = Qt::Vertical;
    d_distros += QString("a");
    d_distros += QString("b");
    d_distros += QString("c");
    d_distros += QString("d");
    ui->qwtPlot->setAxisScaleDraw( QwtPlot::xBottom, new YourScaleDraw( orientation, d_distros ) );
    //重绘 x轴  实现二:
    QDateTime my_basetime;
    my_basetime = QDateTime::currentDateTime();
    ui->qwtPlot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScakeDrawR1( my_basetime ) );
    ui->qwtPlot->setAxisLabelRotation( QwtPlot::xBottom, 60 );
    ui->qwtPlot->setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignRight | Qt::AlignBottom );
    //当X轴重绘,如果需要picker显示时间会用到下面的指令
//    PlotPickerByTime *datetimepicker;
//    datetimepicker = new PlotPickerByTime(QwtPlot::xBottom, QwtPlot::yLeft,
//                                          QwtPicker::CrossRubberBand,
//                                          QwtPicker::AlwaysOn, ui->qwtPlot->canvas());
//    datetimepicker->setTrackerPen(QPen(QColor(255, 0, 0)));

    //计算时间差
    QTime my_start_time(0,0,0,0);
    QTime my_end_time(0,0,1,10);
    int timeElapsed = my_start_time.msecsTo(my_end_time);
    qDebug() << "时间差(时):" << timeElapsed << "ms" << endl;
    QDate my_start_date(2018, 6, 10);
    QDate my_end_date(2018, 6, 11);
    int dateElapset = my_start_date.daysTo(my_end_date);
    qDebug() << "时间差(天):" << dateElapset << "天" << endl;
    QDateTime my_start_datetime( my_start_date, my_start_time );
    QDateTime my_end_datetime( my_end_date, my_end_time );
    int datetimeElapset = my_start_datetime.secsTo( my_end_datetime );
    qDebug() << "时间差(天.时):" << datetimeElapset << "s" << endl;



    //缩放
    QwtPlotMagnifier *magnifier;
    magnifier = new QwtPlotMagnifier(ui->qwtPlot->canvas());
    magnifier->setWheelFactor( 1.1 );  //设置缩放步进,同时与鼠标滚轮上下滚动与缩放方向有关

    //平移
    QwtPlotPanner *panner;
    panner = new QwtPlotPanner(ui->qwtPlot->canvas());
    //panner->setMouseButton( Qt::RightButton );  //设置哪个键平移画布

    //局部缩放
    QwtPlotZoomer *zoomer = new QwtPlotZoomer( ui->qwtPlot->canvas() );
    zoomer->setZoomBase( QRectF(0,0,6,6) );  //矩形:左上(0,0),100为长宽,这个尺寸是图像复原的尺寸,使用这个操作需要设置坐标刻度ui->qwtPlot->setAxisScale,不然使用默认复原尺寸
    //zoomer->setRubberBandPen( QColor( Qt::blue ) );  //设置选择放大区域的边框颜色
    //zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );  //ctrl+右键==恢复到原始状态  MouseSelect2模式:默认左键缩放,恢复方式可以自定义
    //zoomer->setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );  //右键==恢复到上一次扩大之前的状态,默认左键放大,右键恢复原样,恢复上一级原样可以自方式定义
    //模式一,默认右键恢复原样,放大的方式可以自定义
    //zoomer->setMousePattern( QwtEventPattern::MouseSelect1, Qt::LeftButton ); //左键放大,右键恢复原样
    zoomer->setMousePattern( QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier ); //ctrl+左键放大,右键恢复原样
    //模式二,默认左键放大,恢复原样的方式可以自定义
    //模式三,默认左键放大,右键恢复原样,恢复上一级的方式可以自定义

    //设置画笔
    QPen pickerpen;
    pickerpen.setColor(Qt::black);

    //显示鼠标所在位置的坐标
    QwtPlotPicker *picker;
    picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::CrossRubberBand,
                               QwtPicker::AlwaysOn, ui->qwtPlot->canvas());
    picker->setTrackerPen(pickerpen);

    //画线
    QVector<double> x,y1,y2; //模拟数据
    x.push_back(1);
    x.push_back(2);
    x.push_back(3);
    y1.push_back(1);
    y1.push_back(1.5);
    y1.push_back(3);
    y2.push_back(1);
    y2.push_back(2.5);
    y2.push_back(3);
    QwtPlotCurve *curve, *curve2;  //创建曲线
    curve = new QwtPlotCurve();
    curve->setTitle("长度");     //曲线名字
    curve->setPen(Qt::red, 1);  //曲线颜色,宽度
    //curve->setStyle(QwtPlotCurve::Dots);  //散点图
    //curve->setCurveAttribute(QwtPlotCurve::Lines, true);
    curve->setSamples(x, y1);  //曲线数据及绘制
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);  //反锯齿
    curve->setSymbol(new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush, QPen( Qt::color1), QSize(10, 10)));
    curve->attach(ui->qwtPlot);
    curve2 = new QwtPlotCurve();  //第二条曲线
    curve2->setPen(Qt::cyan, 2, Qt::DashLine);  //设置线的颜色,宽度,线型
    curve2->setTitle("质量");
    curve2->setCurveAttribute( QwtPlotCurve::CurveAttribute( QwtPlotCurve::Fitted ) );  //设置为平滑曲线
    //curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );   //不知道有啥用
    curve2->setLegendAttribute( curve2->LegendShowLine );  //设置legenditem使用的是颜色区分还是别的信息区分
    curve2->setSamples(x, y2);
    curve2->attach(ui->qwtPlot);
    //curve2->detach();  //取消线条

    //插入图例legenditem
    QwtPlotLegendItem *d_qwtplotlegenditem1;
    d_qwtplotlegenditem1 = new QwtPlotLegendItem;
    d_qwtplotlegenditem1->setAlignment( Qt::AlignRight | Qt::AlignTop);  // 右上角对齐
    d_qwtplotlegenditem1->setMaxColumns( 1 );  //设置最大列数
    d_qwtplotlegenditem1->setBorderPen( QPen(QColor(255, 0, 0)) );  //设置边缘画笔
    d_qwtplotlegenditem1->setBackgroundBrush( QBrush(QColor(255, 255, 0, 100)) );   //设置背景笔刷, 透明度也在这里设置
    d_qwtplotlegenditem1->setBackgroundMode( QwtPlotLegendItem::LegendBackground );  //画一个框还是分别画框
    d_qwtplotlegenditem1->setTextPen( QPen(QColor(255, 0, 0)) );   //设置文本颜色
    d_qwtplotlegenditem1->setItemMargin(20);  //设置边缘
    d_qwtplotlegenditem1->setItemInterest( QwtPlotItem::ScaleInterest );     //不知道有啥用
    d_qwtplotlegenditem1->setItemSpacing(20);   //下边没注释的没仔细区分过功能
    d_qwtplotlegenditem1->setMargin(10);
    d_qwtplotlegenditem1->setSpacing(20);
    d_qwtplotlegenditem1->setBorderDistance(20);
    d_qwtplotlegenditem1->setBorderRadius(9);
    d_qwtplotlegenditem1->setItemAttribute( QwtPlotItem::AutoScale );
    d_qwtplotlegenditem1->attach(ui->qwtPlot);

    //插入图例 legend
    QwtLegend *legend;
    legend = new QwtLegend;
    //legend->setFrameShape();
    ui->qwtPlot->insertLegend(legend, QwtPlot::BottomLegend);  //插入到qwtplot里面  设置插入位置

    //qwtmarker 标注图中的一个点
    QwtPlotMarker *qwtplotmarker;
    qwtplotmarker = new QwtPlotMarker( "one" );
    QwtText title1;
    title1.setText("平均值");
    QFont font3("宋体",30);
    title1.setFont(font3);
    title1.setColor( Qt::green );
    qwtplotmarker->setTitle("two");
    //qwtplotmarker->setLabel( QwtText("mark" ) ); //标记内容
    qwtplotmarker->setLabel(title1);  //如果需要更改标记内容的大小,在这里更改
    qwtplotmarker->setValue( QPointF(2, 2) );  //要标记的点
    qwtplotmarker->setItemAttribute(QwtPlotItem::AutoScale, true);  //设置marker的自动标尺
    qwtplotmarker->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );  //对齐
    qwtplotmarker->setLabelOrientation(Qt::Horizontal);  //设置字体水平垂直
    qwtplotmarker->setLineStyle( QwtPlotMarker::LineStyle::HLine
                                 );  //设置标记风格
    qwtplotmarker->setLinePen(QPen( Qt::red, 1, Qt::DashDotDotLine ));  //设置marker颜色和线型
    qwtplotmarker->setSymbol( new QwtSymbol( QwtSymbol::DTriangle, Qt::NoBrush, QPen( Qt::color1), QSize(10, 10)) );  //被标记点的符号
    qwtplotmarker->attach(ui->qwtPlot);

    //可能会有用的操作,直接操作plot中的对象
    QwtPlotItemList items = ui->qwtPlot->itemList( QwtPlotItem::Rtti_PlotCurve );  //获取程序运行时的曲线信息,可以使用size函数获得曲线数量
    items[0]->setLegendIconSize(QSize( 10, 10 ));   //设置图标大小
    items[0]->setVisible(true);   //设置折线是否可见

    //调色板  下面没用
    QPalette d_palette;
    d_palette.setColor(QPalette::Background, QColor(255, 0, 0, 255));  //背景色
    d_palette.setColor(QPalette::Foreground, QColor(0, 255, 0, 255));  //前景色

    //画多条曲线,使用for循环里的同一个curve
//    QVector<QVector<double>> y3;
//    y3.push_back(y1);
//    y3.push_back(y2);
//    for(int i = 0; i < 2; i++){
//        QwtPlotCurve *curve;
//        QColor d_color;
//        curve = new QwtPlotCurve();
//        d_color.setHsl((25*i)%255, 255, ((i/10+1)*80)%255);
//        curve->setPen( d_color, 3);
//        curve->setTitle("质量");
//        curve->setSamples(x, y3[i]);
//        curve->attach(ui->qwtPlot);
//    }

    //重新绘制
    ui->qwtPlot->replot();

    //保存图片
    QwtPlotRenderer renderer;
    //renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
    renderer.setDiscardFlags(QwtPlotRenderer::DiscardBackground | QwtPlotRenderer::DiscardCanvasBackground);  //去掉背景和画布颜色
    //renderer.setLayoutFlag( QwtPlotRenderer::DefaultLayout );
    renderer.renderDocument(ui->qwtPlot, "plot1.tif", QSizeF(200, 150), 300);//  png tif

    /*************************************  绘制 qwtplot2 (多柱柱状图)  *********************************************/
    ui->qwtPlot_2->detachItems();  //清理绑定数据
    ui->qwtPlot_2->setTitle(QString("标题"));  //设置标题

    //设置画布
    ui->qwtPlot_2->setAutoFillBackground(true);  //填充背景
    ui->qwtPlot_2->setPalette(QColor("Linen"));  //设置背景颜色

    int axis1 = QwtPlot::xBottom;
    int axis2 = QwtPlot::yLeft;
    ui->qwtPlot_2->setAxisTitle(axis1, "This is X");   //X轴轴标
    ui->qwtPlot_2->setAxisMaxMinor(axis1, 4);   //设置某个某个坐标轴缩小比例尺的最大间隔数目  (axisl轴显示刻度的多少)
    ui->qwtPlot_2->setAxisMaxMajor(axis1, 5);   //设置某个某个坐标轴扩大比例尺的最大间隔数目  (axisl轴显示给定个数的刻度数字)
    //ui->qwtPlot_2->setAxisScaleDraw(axis1, new QwtScaleDraw);
    ui->qwtPlot_2->setAxisTitle(axis2, "This is Y");   //Y轴轴标
//    ui->qwtPlot_2->setAxisMaxMinor(axis2, 3);   //设置某个某个坐标轴缩小比例尺的最大间隔数目
//    ui->qwtPlot_2->setAxisMaxMajor(axis2, 3);   //设置某个某个坐标轴扩大比例尺的最大间隔数目

    //设置MultiBarChart  可以理解为plot的一个组件  柱状图画多个柱的组件
    QwtPlotMultiBarChart *d_barChartItem;
    d_barChartItem = new QwtPlotMultiBarChart( "Bar Chart " );   //柱状图表
    d_barChartItem->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );  //不知道有啥子用
    d_barChartItem->setSpacing( 20 );  //设置两组柱状图之间的距离
    d_barChartItem->setMargin( 3 );  //设置柱状图两端距离边缘的宽度
    d_barChartItem->attach(ui->qwtPlot_2);

    //添加图例   另一种方法是使用qwtlegend,但这个与plot不是一个系列,无法嵌入
    QwtPlotLegendItem *d_qwtplotlegenditem;
    d_qwtplotlegenditem = new QwtPlotLegendItem;
    d_qwtplotlegenditem->setAlignment( Qt::AlignRight | Qt::AlignTop);  // 右上角对齐
    d_qwtplotlegenditem->setMaxColumns( 1 );  //设置最大列数
    d_qwtplotlegenditem->attach(ui->qwtPlot_2);

    static const char *colors[] = { "DarkOrchid", "SteelBlue", "Gold" };  //每组柱状图的三种颜色

    const int numSamples = 5;  //设置5组柱状图
    const int numBars = sizeof( colors ) / sizeof( colors[0] );

    //设置旁边的图标说明,比如说金色代表什么意思,蓝色代表什么意思
    QList<QwtText> titles;
    for ( int i = 0; i < numBars; i++ )
    {
        QString title("Bar %1");
        titles += title.arg( i );
    }
    d_barChartItem->setBarTitles( titles );
    d_barChartItem->setLegendIconSize( QSize( 10, 14 ) );

    //设置柱状图的风格和颜色
    for ( int i = 0; i < numBars; i++ )
    {
        QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );
        symbol->setPalette( QPalette( colors[i] ) );

        d_barChartItem->setSymbol( i, symbol );
    }

    //为柱状图添加数据
    QVector< QVector<double> > series;
    for ( int i = 0; i < numSamples; i++ )
    {
        QVector<double> values;
        for ( int j = 0; j < numBars; j++ )
            values += ( 2 + qrand() % 8 );

        series += values;
    }

    d_barChartItem->setSamples( series );

    /*************************************  绘制 qwtplot3 (单柱柱状图)  *********************************************/
    ui->qwtPlot_3->detachItems();  //清理绑定数据
    ui->qwtPlot_3->setTitle(QString("单柱柱状图"));  //设置标题

    //设置画布
    ui->qwtPlot_3->setAutoFillBackground(true);  //填充背景
    ui->qwtPlot_3->setPalette(QColor("Linen"));  //设置背景颜色


    //QwtPlotBarChart  柱状图只画一个柱的组件
    QwtPlotBarChart *d_qwtbarchar;
    d_qwtbarchar = new QwtPlotBarChart(" Single Bar Chart ");

    QwtColumnSymbol *singlebar_symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
    singlebar_symbol->setLineWidth( 2 );
    singlebar_symbol->setFrameStyle( QwtColumnSymbol::Raised );
    singlebar_symbol->setPalette( QPalette( Qt::red ) );
    d_qwtbarchar->setSymbol( singlebar_symbol );

    d_qwtbarchar->setSamples( series.at(0) );
    d_qwtbarchar->attach(ui->qwtPlot_3);

    ui->qwtPlot_3->setAxisLabelRotation( QwtPlot::xBottom, -30 );  //旋转X轴的标注旋转角度

//    QwtScaleDraw *single_bar_scaledraw;
//    single_bar_scaledraw = new QwtScaleDraw();
//    //single_bar_scaledraw
//    ui->qwtPlot_3->setAxisScaleDraw( QwtPlot::xBottom, sdf );

    /*************************************  绘制 qwtplot4 (单柱多色自定义柱状图)  ****************************************/

    ui->qwtPlot_4->detachItems();
    QList<QString> d_distros1;
    QVector<QString> Vdistro;
    QVector<double> Vhits;
    QVector<QColor> VQColor;
    for(int i = 0; i < 8; i++){
        Vdistro.push_back( QString::number(i+2) );
        Vhits.push_back(i*1000);
        VQColor.push_back( QColor(10, 25, i*10) );
    }

    ui->qwtPlot_4->setAutoFillBackground( true );
    ui->qwtPlot_4->setPalette( QColor( "Linen" ) );

    QwtPlotCanvas *canvas2;
    canvas2 = new QwtPlotCanvas();
    canvas2->setFrameStyle( QFrame::NoFrame );
    canvas2->setBorderRadius( 10 );
    ui->qwtPlot_4->setCanvas( canvas2 );
    ui->qwtPlot_4->setTitle( "单柱多色柱状图" );

    QwtPlotBarChart1 *d_barChart;
    d_barChart = new QwtPlotBarChart1();

    for( uint i = 0; i < Vdistro.size(); i++ ){
        d_distros1 += Vdistro.at(i);
        d_barChart->addbarchart( Vdistro.at(i), VQColor.at(i) );  //legend数据和
    }
    d_barChart->setSamples( Vhits );
    d_barChart->attach(ui->qwtPlot_4);

    //插入legenditem
    QwtPlotLegendItem *d_qwtplotlegenditem2;
    d_qwtplotlegenditem2 = new QwtPlotLegendItem;
    d_qwtplotlegenditem2->setAlignment( Qt::AlignRight | Qt::AlignTop);  // 右上角对齐
    d_qwtplotlegenditem2->setMaxColumns( 1 );  //设置最大列数
    d_qwtplotlegenditem2->attach(ui->qwtPlot_4);

    ui->qwtPlot_4->replot();

    /************************************************ Qmap  ***********************************************/
    //添加：
    QMap<QString, QString> map;  //!!!!!说明:  1. QMap<class Key, class T>  尖括号里的内容可以是别的类型
    map.insert("3name", "leo");  //           2.  如果 class T 的类型不确定,可以使用QVariant,这种类型可以存放各种别的类型的数据
    map.insert("1age", "18");    //               使用方法 QMap<   , QVariant >
    map.insert("2like", "eat");
    map.insert("4sex", "man");
    map["5mine"] = QString( "jiji" );
    //遍历：
    //QMap<QString, QString>::const_iterator i;
    for( QMap<QString, QString>::const_iterator i=map.constBegin(); i!=map.constEnd(); ++i)
        qDebug() << i.key() <<"        " << i.value();
    //QMap的Key会自动按升序排列

    qDebug() << "---------------------------------";

    QMap<QString, QString>::iterator mi;
    //查找：
    mi = map.find("2like");  //在此步已经得到遍历结果,下面不是遍历过程
    qDebug() << mi.key() <<"   --     " << mi.value();
    if(++mi != map.end()){
        ++mi;
        qDebug() << mi.key() <<"  ++      " << mi.value();
    }

    QMap <QString, QwtPlotCurve *> map_curve;
    map_curve["curve"] = curve;
    map_curve["curve2"] = curve2;
    //map_curve["curve2"]->detach();

    /***************************************************** 文件操作 **************************************************/
    std::ifstream inFile("test.txt", std::ios::in );
    if(!inFile.is_open()){
        qDebug() << "File read error" << endl;
        return;
    }
    double tmp;
    char c; //用来检查回车符号
    //检查列
    int count;
    while(1){
        inFile >> tmp;
        c = inFile.peek();  //peek()只检查,不会让流前进,而get会让流前进
        if( c == '\n' ){
            break;
        }
        count++;
    }
    qDebug() << "File rows is" << count << endl;
    inFile.seekg(0); //游标复位
    inFile >> tmp;  //显示下一个值,次数关于游标设置
    //qDebug() << "next " << tmp << endl;
    inFile.close();

    /************************************************* MySQL 数据库操作 *************************************************/
//    QSqlDatabase Mysqldb;
//    //Mysqldb = new QSqlDatabase::addDatabase("MYSQL");
//    Mysqldb.setHostName("127.0.0.1");
//    Mysqldb.setUserName("root");
//    Mysqldb.setPassword("wenyue");
//    Mysqldb.setDatabaseName("wenyue_test");
//    QSqlTableModel *model = new QSqlTableModel(this);
//    model->setTable("test1");
//    //ui->listWidget->setModel(model);
//    model->select();
/*
    //使用 mysql 的API实现   一个不错的博客  https://www.cnblogs.com/47088845/p/5706496.html
    //----  初始化  连接数据库
    MYSQL mysql_conn;
    int result;   //用于后边记录数据库操作返回值,是否成功
    MYSQL_RES *res;  //这个结构代表返回行的一个查询结果集
    mysql_init( &mysql_conn );  //连接数据库
    if( mysql_real_connect( &mysql_conn, "localhost", "root", "wenyue", "wenyue_test", 0, NULL, CLIENT_FOUND_ROWS ) ){
        qDebug() << "MySQL connect succeed!" << endl;
    }
    //----  用户操作
    result = mysql_query( &mysql_conn, "insert into test1 values(7, 8, 9)" );
    if(result) qDebug()<< "insert error" << endl;
    result = mysql_query( &mysql_conn, "delete from test1 where ID = 7" );
    if(result) qDebug()<< "query date error" << endl;
    //----  关闭数据库
    mysql_free_result(res);
    mysql_close( &mysql_conn );
*/
    /************************************************  定时器  *******************************************************/
    //方法一
    my_timerID = startTimer( 3000 ); //此处数值为毫秒  3秒
    //killTimer( my_timerID );  //可以通过这条指令杀死定时器
    //方法二
    my_timer2 = new QTimer( this );
    connect( my_timer2, SIGNAL(timeout()), this, SLOT(timer2solt()) );
    my_timer2->start( 3000 );  //以毫秒为单位

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //设置当前tab
    ui->tabWidget->setCurrentWidget(ui->tab_2);

    //设置comboBox
    ui->comboBox->addItem("hello");
    ui->comboBox->addItem("two");
    ui->comboBox->addItem("three");

    //设置listwidget
    ui->listWidget->addItem("test");
    ui->listWidget->addItem("test1");
    ui->listWidget->addItem("test2");

    //将string转成double
    std::string a("123.6");
    double b;
    b = std::stod(a);
    //std::cout << b << std::endl;
    //将string vector转成double vector
    std::vector<std::string> aa;
    aa.push_back("111.11");
    aa.push_back("222.22");
    aa.push_back("333.33");
    std::vector<double> bb(aa.size());
    std::transform(aa.begin(), aa.end(), bb.begin(), [](const std::string& val)
    {
        return std::stod(val);
    });
    //std::cout << bb.at(0) << " " << bb.at(1) << " " << bb.at(2) << std::endl;

    //string转成Qstring
    std::string mystr("11.12");
    QString myqstr;
    myqstr=QString::fromStdString(mystr);

    //string转到double
    double mydouble;
    mydouble=myqstr.toDouble();
    //qDebug()<<"mydouble"<<mydouble;

    //double转到Qstring
    qDebug()<<"Qstr"<< QString::number(mydouble,'g',6);
   // qDebug() << QString::fromStdString( mydouble.toStdString() ) << endl;

    //vector越界,可能存在的原因是访问的位置没有值,如下,可以使用迭代器返回,不会存在越界的问题,但是空的vector会返回0
//    QVector<double> test;
//    double aaa;
//    if(test.size() == 0){
//        qDebug() << "test is empty" << endl;
//        QMessageBox::information(this,"error","test is empty");  //弹窗
//        return;
//    }
//    //aaa = test.at(0);
//    aaa = *test.begin();
//    qDebug() << aaa ;

    //emit ui->pushButton->clicked();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent( QTimerEvent *event )
{
    qDebug() << "hello~ I'm timer" << endl;
}

void MainWindow::on_comboBox_activated(int index)
{
    switch (index) {
    case 0:ui->tabWidget->setCurrentWidget(ui->tab);break;
    case 1:ui->tabWidget->setCurrentWidget(ui->tab_2);break;
    case 2:ui->tabWidget->setCurrentWidget(ui->tab_3);break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->listWidget->selectAll();
    qDebug() << "Selected All" << endl;
}

void MainWindow::on_pushButton_2_clicked()
{
    QList<QListWidgetItem*> selectedLW = ui->listWidget->selectedItems();
    qDebug() << "Selected num " << selectedLW.size() << endl;
}

void MainWindow::timer2solt()
{
    qDebug() << "hello~ I'm timer 2" << endl;
}
