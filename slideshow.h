#ifndef SLIDESHOW_H
#define SLIDESHOW_H
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QPixmap>
class Timer;

class slideshow:public QObject
{
    Q_OBJECT
private:
    bool mRunning;
    int mSlidesCount;
    int mCurrentSlideIndex;
    QMap<int, QByteArray> mSlidePreviewsBytes;
    QMap<int, QString> mSlideNotes;
    QMap<int, QPixmap> mPixmaps;
public:
    explicit slideshow(QObject*parent = nullptr);
    void setRunning(bool aRunning);
    bool isRunning()const;
    void setSlidesCount(int aSlidesCount);
    int getSlidesCount() const;
    void setCurrentSlideIndex(int aCurrentSlideIndex);
    int getCurrentSlideIndex()const;
    int getHumanCurrentSlideIndex()const;
    void setSlidePreviewBytes(int aSlideIndex, const QByteArray& aSlidePreviewBytes);
    QByteArray getSlidePreviewBytes(int aSlideIndex) const;
    void setSlideNotes(int aSlideIndex, const QString &aSlideNotes);
    QString getSlideNotes(int aSlideIndex)const;
    void reset();
    void setPixmap(int index, const QPixmap& pixmap);
    QPixmap getPixmap(int index) const;
};

#endif // SLIDESHOW_H
