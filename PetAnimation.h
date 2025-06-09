#ifndef PETANIMATION_H
#define PETANIMATION_H

//动画播放控制
#pragma once
#include <QObject>
#include <QVector>
#include <QPixmap>
class AnimationSystem : public QObject {
    Q_OBJECT
public:
    void loadAnimation(const QString &name, const QVector<QPixmap> &frames);
    void play(const QString &animationName);

private:
    //QMap<QString, QVector<QPixmap>> m_animations;
    int m_currentFrame = 0;
};
#endif // PETANIMATION_H
