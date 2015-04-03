#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtCore/QObject>
#include <QtWidgets\QMainWindow>
#include <QtGui/QtGui>
namespace Strategy {
  class BeliefState;
};
class MainWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MainWidget(Strategy::BeliefState *state, QWidget *parent = 0);
  ~MainWidget();
  enum Shape { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc,
               Chord, Pie, Path, Text, Pixmap
             };
  void setName(const QString &name);
  QString name() const;
private:
  QTimer *timer;
  Shape shape;
  QPen pen;
  QBrush brush;
  bool antialiased;
  bool transformed;
  QPixmap pixmap;
  Strategy::BeliefState *state;
protected:
  void paintEvent(QPaintEvent *event);
};
#endif // MAINWIDGET_H
