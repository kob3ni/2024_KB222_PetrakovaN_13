#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SmartPtr.h"

MainWindow::MainWindow(QWidget *parent) :
 QMainWindow(parent), ui(new Ui::MainWindow) { //Конструктор
 ui->setupUi(this);
 //Настраиваем интерфейс:
 label = new QLabel(ui->scrollArea);
 label->setAlignment(Qt::AlignTop);
 ui->scrollArea->setWidget(label); //Виджетом для scrollArea ставим простую метку :)
 createActions(); //Создаем меню
 repaint(); //Перерисовываем
 instrument = 1; //Выбираем карандаш
 color.setRgb(0, 0, 0); //Черный цвет
 isStPolgn = false; //Полигон не начат
 isDblClick = false; //Двойного щелчка не было
 widthOfPen = 3; //Ставим ширину кисти
 changed = false; //Изменений не было
 newFile(); //Новый файл
}

//Выбор инструментов, слоты
void MainWindow::pen() { instrument = 1; repaintButtons (); }
void MainWindow::lastic() { instrument = 2; repaintButtons (); }
void MainWindow::line() { instrument = 3; repaintButtons (); }
void MainWindow::ellipse() { instrument = 4; repaintButtons (); }
void MainWindow::rectangle() { instrument = 5; repaintButtons (); }
void MainWindow::polygon() {
 instrument = 6; isStPolgn = true; repaintButtons ();
}
void MainWindow::setColor() { //Выбор цвета
 const QColor newColor = QColorDialog::getColor(color); //Диалог выбора цвета
 if (newColor.isValid()) {
  color = newColor; //Ставим цвет, если он валидный
  int r1 = (newColor.red() + 0x80) % 0x100,
      g1 = (newColor.green() + 0x80) % 0x100,
      b1 = (newColor.blue() + 0x80) % 0x100;
  const QColor invertColor = QColor(r1,g1,b1); //Вычисляем обратный цвет для надписи
  ui->pushButton->setStyleSheet(
   QString("background-color: %1; color: rgb(%2, %3, %4);").
    arg(newColor.name()).arg(invertColor.red()).
    arg(invertColor.green()).
    arg(invertColor.blue())); //Меняем цвет и фон кнопки
 }
 repaint();
}

void MainWindow::repaintButtons () { //Изменить фон активной кнопки
 QColor selectedColor(153,153,153),
        defaultColor(255,255,255);
 QList <QPushButton *> butts = this->findChildren<QPushButton *>();
 for (int b = 0; b < butts.size(); b++) {
  if (butts[b]->objectName().contains("_")) {
   //Имена кнопок по шаблону pushButton_1, pushButton_2, ...
   if (butts[b]->objectName() == QString("pushButton_%1").arg(instrument))
    butts[b]->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").
     arg(selectedColor.red()).arg(selectedColor.green()).arg(selectedColor.blue()));
   else
    butts[b]->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").
     arg(defaultColor.red()).arg(defaultColor.green()).arg(defaultColor.blue()));
  }
 }
}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
 //Событие нажатия кнопки мыши, расчет координат под курсором
 isPressed = true;
 if (instrument == 6) { //Если выбран полигон
  if (isDblClick) isStPolgn = true;
  if (isStPolgn==true) {
   current = ev->pos() - ui->scrollArea->geometry().topLeft() +
             QPoint(ui->scrollArea->horizontalScrollBar()->value(),
                    ui->scrollArea->verticalScrollBar()->value());
  }
  isDblClick = false;
 }
 else {
  current = ev->pos() - ui->scrollArea->geometry().topLeft() +
            QPoint(ui->scrollArea->horizontalScrollBar()->value(),
            ui->scrollArea->verticalScrollBar()->value());
 }
 next = current;
 repaint();
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev) { //Событие движения курсора, расчет координат под курсором
  next = ev->pos() - ui->scrollArea->geometry().topLeft() +
         QPoint(ui->scrollArea->horizontalScrollBar()->value(),
                ui->scrollArea->verticalScrollBar()->value());
  ui->statusBar->showMessage((changed==true?QString("* "):QString(""))+
   QString::number(next.x())+QString(",")+QString::number(next.y())
  );
  repaint();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev) { //Событие отпускания зажатой кнопки мыши, отрисовываем примитивы
 isPressed = false;
 next = ev->pos() - ui->scrollArea->geometry().topLeft() +
        QPoint(ui->scrollArea->horizontalScrollBar()->value(),
        ui->scrollArea->verticalScrollBar()->value());
 if (instrument == 3) {
  drawLine();
 }
 if (instrument == 4) {
  drawEllipse();
 }
 if (instrument == 5) {
  drawRectangle();
 }
 if (instrument == 6) {
  drawPolygon();
 }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *) { //Двойной клик, завершаем полигон
 if (instrument == 6) {
  painter.begin(&img);
  painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
  painter.drawLine(current, currentTmp);
  painter.end();
  label->setPixmap(img);
 }
 isDblClick = true;
 repaint();
}

void MainWindow::paintEvent(QPaintEvent *) { //Отрисовка (событие)
 if (current == next) return; //Координаты не менялись - выйти
 if (instrument == 1) { //Если выбран карандаш
  if (isPressed) { //Если кнопка зажата
   painter.begin(&img); //Рисуем по координатам
   painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
   changed = true;
   painter.drawLine(current, next);
   painter.end();
  }
  current = next;
  label->setPixmap(img);
 }
 if (instrument == 2) { //Если ластик
  if (isPressed) {
   painter.begin(&img);
   painter.setPen(QPen(Qt::white, widthOfPen + 6, Qt::SolidLine));
    //Рисуем белым цветом
   changed = true;
   painter.drawLine(current, next);
   painter.end();
  }
  current = next;
  label->setPixmap(img);
 }
 if (instrument == 3) { //Если линия
  imgTmp = img;
  if (isPressed) {
   painter.begin(&imgTmp);
   painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
   if (current != next) painter.drawLine(current, next); //Рисуем линию
   painter.end();
   label->setPixmap(imgTmp);
  }
 }
 if (instrument == 4) { //Если эллипс
  imgTmp = img;
  if (isPressed) {
   painter.begin(&imgTmp);
   painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
   painter.drawEllipse(QPointF(current), next.x() - current.x(), next.y() - current.y());
   painter.end();
   label->setPixmap(imgTmp);
  }
 }
 if (instrument == 5) { //Если прямоугольник
  imgTmp = img;
  if (isPressed) {
   painter.begin(&imgTmp);
   painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
   painter.drawRect(current.x(), current.y(),
    next.x() - current.x(), next.y() - current.y());
   painter.end();
   label->setPixmap(imgTmp);
  }
 }
 if (instrument == 6) { //Если полигон
  imgTmp = img;
  if (isPressed) {
   painter.begin(&imgTmp);
   painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
   if (current != next) painter.drawLine(current, next);
   painter.end();
   label->setPixmap(imgTmp);
  }
 }
}

//Отрисовка разных примитивов
void MainWindow::drawLine() {
 painter.begin(&img);
 painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
 if (current != next) { changed = true; painter.drawLine(current, next); }
 painter.end();
 label->setPixmap(img);
 repaint();
}

void MainWindow::drawEllipse() {
 painter.begin(&img);
 painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
 if (current != next) {
  changed = true;
  painter.drawEllipse(QPointF(current), next.x() - current.x(), next.y() - current.y());
 }
 painter.end();
 label->setPixmap(img);
 repaint();
}

void MainWindow::drawRectangle() {
 painter.begin(&img);
 painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
 if (current != next) {
  changed = true;
  painter.drawRect(current.x(), current.y(),
   next.x() - current.x(), next.y() - current.y());
 }
 painter.end();
 label->setPixmap(img);
 repaint();
}

void MainWindow::drawPolygon() {
 if (isStPolgn) currentTmp = current;
 painter.begin(&img);
 painter.setPen(QPen(color, widthOfPen, Qt::SolidLine));
 if (current != next) { changed = true; painter.drawLine(current, next); }
 painter.end();
 label->setPixmap(img);
 current = next;
 isStPolgn = false;
 repaint();
}

void MainWindow::newFile() { //Новый файл, открываем файл шаблона
 if (changed) save();
 loadFile(":/template.png");
}

void MainWindow::open() { //Открыть файл, открываем диалог, выбираем файл
 if (changed) save();
 const QString fileName = QFileDialog::getOpenFileName(
  this, tr("Открыть файл"), QDir::currentPath(),tr("Изображения (*.png *.bmp *.jpg);;Все файлы (*.*)"));
 if (!fileName.isEmpty()) {
  QFileInfo fi(fileName);
  QDir::setCurrent(fi.canonicalPath());
  loadFile(fileName);
 }
}

bool MainWindow::save() { //Сохранение, открываем диалог, показываем, куда сохранять
 bool b=false;
 const QString initialPath = QDir::currentPath() + "/untitled.png";
 const QString fileName =
  QFileDialog::getSaveFileName(this, tr("Сохранить как"), initialPath,
   tr("Изображения PNG (*.png);;Изображения BMP (*.bmp);;")+
   tr("Изображения JPG (*.jpg);;Все файлы (*.*)"));
 if (!fileName.isEmpty()) {
  QFileInfo fi(fileName);
  b = img.save(fileName, fi.completeSuffix().toStdString().c_str());
  if (b) changed = false;
  else QMessageBox::critical(this, tr("Ошибка"), tr("Не могу сохранить файл"));
 }
 return b;
}

void MainWindow::loadFile(const QString &fileName) { //Загрузка файла
 if (!fileName.isEmpty()) {
  bool b = img.load(fileName);
  if (b) {
   label->setPixmap(img);
   changed = false;
  }
  else {
   QMessageBox::critical(this, tr("Ошибка"), tr("Не могу открыть файл"));
  }
 }
}

void MainWindow::createActions() { //Создаем меню "Файл"
 QMenu *fileMenu = menuBar()->addMenu(tr("&Файл"));
 QAction *newAct = fileMenu->addAction(tr("&Новый файл"));
 newAct->setShortcuts(QKeySequence::New);
 connect(newAct, SIGNAL(triggered(bool)), SLOT(newFile()));

 QAction *openAct = fileMenu->addAction(tr("&Открыть файл"));
 openAct->setShortcuts(QKeySequence::Open);
 connect(openAct, SIGNAL(triggered(bool)), SLOT(open()));

 QAction *saveAct = fileMenu->addAction(tr("&Сохранить как"));
 saveAct->setShortcuts(QKeySequence::Save);
 connect(saveAct, SIGNAL(triggered(bool)), SLOT(save()));

 QAction *quitAct = fileMenu->addAction(tr("&Выход"));
 quitAct->setShortcuts(QKeySequence::Quit);
 connect(quitAct, SIGNAL(triggered(bool)), SLOT(close()));
}

MainWindow::~MainWindow() { delete ui; } //Деструктор, удаляем интерфейс

void MainWindow::on_spinBox_valueChanged(int arg1)
{
     widthOfPen = arg1;
}
