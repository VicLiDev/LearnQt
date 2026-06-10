//--------------设置图例可以被点击来确定是否显示曲线-----------------------//
    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode( QwtLegendData::Checkable );//图例可被点击
    insertLegend( legend, QwtPlot::RightLegend );
    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
        SLOT( showItem( const QVariant &, bool ) ) );//点击图例操作

    QwtPlotItemList items = itemList( QwtPlotItem::Rtti_PlotCurve );//获取画了多少条曲线,如果为获取其他形状，注意改变参数
   //  qDebug()<<items;
    for ( int i = 0; i < items.size(); i++ )
    {

        if ( i == 0 )
        {
            const QVariant itemInfo = itemToInfo( items[i] );

            QwtLegendLabel *legendLabel =
                qobject_cast<QwtLegendLabel *>( legend->legendWidget( itemInfo ) );
            if ( legendLabel )
                legendLabel->setChecked( true );//

            items[i]->setVisible( true );
        }
        else
        {
            items[i]->setVisible( false );
        }
    }


    this->resize(600,400);

    this->replot();

    setAutoReplot( true );//设置自动重画，相当于更新

}
//点击图例，显示相应的曲线
void PlotLines::showItem(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if ( plotItem )
        plotItem->setVisible( on );
}
