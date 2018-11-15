#include <QtTest>

// add necessary includes here

class gamestatetest : public QObject
{
    Q_OBJECT

public:
    gamestatetest();
    ~gamestatetest();

private slots:
    void test_case1();

};

gamestatetest::gamestatetest()
{

}

gamestatetest::~gamestatetest()
{

}

void gamestatetest::test_case1()
{

}

QTEST_APPLESS_MAIN(gamestatetest)

#include "tst_gamestatetest.moc"
