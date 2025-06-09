#ifndef PETBASE_H
#define PETBASE_H

//宠物基础属性和核心逻辑
#pragma once
#include <QWidget>
#include <QString>

class PetBase : public QWidget {
    Q_OBJECT
public:
    explicit PetBase(QWidget *parent = nullptr);
    virtual void updatePet() = 0; // 每帧更新

    // 基础属性
    void setName(const QString &name);
    QString name() const;

protected:
    QString m_name;
    QPixmap m_currentSprite;
};
#endif // PETBASE_H
