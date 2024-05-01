#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "SmartPtr.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  QPixmap img; //Изображение и его временная копия
  QPixmap imgTmp;
  bool isPressed; //Нажата ли кнопка мыши
  bool isStPolgn; //Является ли точка началом полигона
  bool isDblClick; //Был ли сейчас двойной клик
  QPointF current; //Различные точки, необходимые для отрисовки
  QPointF next;
  QPointF currentTmp;
  QLabel *label; //Метка, в которой отображается рисунок
  int instrument; //Выбранный инструмент
  QColor color; //Цвет
  QPainter painter; //Объект, который занимается отрисовкой
  int widthOfPen; //Ширина кисти
  bool changed; //Индикатор изменений

protected:
  //События
  void paintEvent(QPaintEvent *); //Событие рисования
  void mouseMoveEvent(QMouseEvent *ev); //...движения курсора
  void mousePressEvent(QMouseEvent *ev); //...нажатия на кнопку мыши
  void mouseReleaseEvent(QMouseEvent *ev); //...отпускания зажатой кнопки мыши
  void mouseDoubleClickEvent(QMouseEvent *); //...двойного нажатия на кнопку мыши

public:
  explicit MainWindow(QWidget *parent = 0); //Конструктор
  ~MainWindow();

private:
  Ui::MainWindow *ui; //Интерфейс
  SmartPtr<QString> ish = new QString("");
  void createActions(); //Метод создания меню
  void loadFile(const QString &fileName); //Метод загрузки файла по пути
  void repaintButtons (); //Подсветка фона активной кнопки

public slots:
  void newFile(); //Новый файл
  void open(); //Открытие
  bool save(); //Сохранение
  void pen(); //Выбор кисти
  void lastic(); //Выбор ластика
  void line(); //Выбор линии
  void drawLine(); //Отрисовка линии
  void ellipse(); //Выбор эллипса
  void drawEllipse(); //Отрисовка эллипса
  void rectangle(); //Выбор прямоугольника
  void drawRectangle(); //Отрисовка прямоугольника
  void polygon(); //Выбор полигона
  void drawPolygon(); //Отрисовка полигона
  void setColor(); //Выбор цвета
  void on_spinBox_valueChanged(int arg1);
};

#endif // MAINWINDOW_H
