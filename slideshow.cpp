#include "slideshow.h"
#include <QDebug>


slideshow::slideshow(QObject*parent)
    :QObject(parent)
    ,mRunning(false)
    ,mSlidesCount(0)
    ,mCurrentSlideIndex(0)
{

}
void slideshow:: setRunning(bool aRunning)
{
    mRunning=aRunning;
}

bool slideshow::isRunning()const
{
    return mRunning;
}

void slideshow:: setSlidesCount(int aSlidesCount)
{
    mSlidesCount=aSlidesCount ;
}

int slideshow:: getSlidesCount() const
{
    return mSlidesCount;
}

void slideshow:: setCurrentSlideIndex(int aCurrentSlideIndex)
{
    mCurrentSlideIndex=aCurrentSlideIndex;
}

int slideshow:: getCurrentSlideIndex()const
{
    return mCurrentSlideIndex;
}

int slideshow:: getHumanCurrentSlideIndex()const
{
    return getCurrentSlideIndex()+1;
}

void slideshow::setSlidePreviewBytes(int aSlideIndex, const QByteArray& aSlidePreviewBytes)
{
    mSlidePreviewsBytes.insert(aSlideIndex, aSlidePreviewBytes);
}

QByteArray slideshow::getSlidePreviewBytes(int aSlideIndex) const
{
    return mSlidePreviewsBytes.value(aSlideIndex);
}

void slideshow::setSlideNotes(int aSlideIndex,const QString &aSlideNotes)
{
    mSlideNotes.insert(aSlideIndex,aSlideNotes);
}

QString slideshow:: getSlideNotes(int aSlideIndex)const
{
    QString aSlideNotes = mSlideNotes.value(aSlideIndex);
    if(aSlideNotes.isNull() ){
        return "";
    }
    return aSlideNotes;
}

void slideshow::reset()
{
    mRunning = false;
    mSlidesCount = 0;
    mCurrentSlideIndex = 0;
    mSlidePreviewsBytes.clear();
    mSlideNotes.clear();
    qDebug() << "Модель презентации сброшена";
}


void slideshow::setPixmap(int index, const QPixmap& pixmap)
{
    mPixmaps[index] = pixmap;
}

QPixmap slideshow::getPixmap(int index) const
{
    return mPixmaps.value(index);
}


