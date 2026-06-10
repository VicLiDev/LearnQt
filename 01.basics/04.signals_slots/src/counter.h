#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>

class Counter : public QObject
{
    Q_OBJECT

public:
    explicit Counter(QObject *parent = nullptr);

    int value() const;

public slots:
    void increment();
    void decrement();
    void reset();

signals:
    void countChanged(int value);

private:
    int m_value;
};

#endif // COUNTER_H
