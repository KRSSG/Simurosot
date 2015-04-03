#include "stdafx.h"
#include "main_widget.h"
#include <QtGui/QtGui>
#include <QtGui/QPaintEvent>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtWidgets\QWidget>
#include <QtWidgets\QGridLayout>
#include <QtGui/QTextLine>
#include "../Skills/skillSet.h"
#include "../Utils/pathPlanners.h"
#include <vector>
#include "../HAL/comm.h"
#include "../common/include/geometry.hpp"
#include "render_area.h"
#include "../common/include/config.h"
#include <iostream>
#include "../Core/beliefState.h"
#include "../HAL/debugCircle.h"
#include "../HAL/debugLines.h"

using namespace std;
MainWidget::MainWidget(Strategy::BeliefState *state_, QWidget *parent) :
  QWidget(parent),
  state(state_)
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(32);
}
int tr_x(int x, float fac) {
  return (HALF_FIELD_MAXX+x)*fac;
}
int tr_y(int y, float fac) {
  return (HALF_FIELD_MAXY-y)*fac;
}
MainWidget::~MainWidget()
{
}
void MainWidget::setName(const QString &name)
{
}
QString MainWidget::name() const
{
  return "Jai Gupta";
}
void MainWidget::paintEvent(QPaintEvent* event)
{
  int my_w = width();
  int my_h = height();
  float fac_w = my_w / (float)HALF_FIELD_MAXX;
  float fac_h = my_h / (float)HALF_FIELD_MAXY;
  float fac = (fac_w < fac_h) ? fac_w : fac_h;
  fac /= 2;
  
  QPainter painter(this);
  painter.setPen(pen);
  painter.setBrush(brush);
  if (antialiased)
    painter.setRenderHint(QPainter::Antialiasing, true);
  QRect rect(-BOT_RADIUS * fac, -BOT_RADIUS * fac, 2*BOT_RADIUS * fac, 2*BOT_RADIUS * fac);
  painter.save();
  painter.fillRect(0, 0, (2*HALF_FIELD_MAXX)*fac, (2*HALF_FIELD_MAXY)*fac, Qt::green);
  painter.restore();
  QColor orange(255,128,128);
  pen.setColor(orange);
  painter.save();
  painter.setPen(pen);
  painter.drawEllipse(tr_x(state->ballPos.x-BALL_RADIUS, fac), 
                      tr_y(state->ballPos.y-BALL_RADIUS, fac), 
                      2*BALL_RADIUS*fac, 2*BALL_RADIUS*fac);
  painter.restore();
  
  // Drawing debug data
  std::list<HAL::Debug_Circle> circles = Strategy::SkillSet::getCircles();
  std::list<HAL::Debug_Line> lines = Strategy::SkillSet::getLines();
  Strategy::SkillSet::clearDebugData();
  QColor black(0,0,0);
  pen.setColor(black);
  painter.setPen(pen);  
  while (circles.empty() == false)
  {
    HAL::Debug_Circle circle = circles.front();
    circles.pop_front();
    QPoint centre(tr_x(circle.x(),fac), tr_y(circle.y(),fac));
    pen.setColor(circle.color());
    painter.setPen(pen);
    painter.save();
    painter.drawEllipse((QPointF)centre, circle.radius()*fac, circle.radius()*fac);
    painter.restore();
  }
  while(lines.empty() == false)
  {
    HAL::Debug_Line line = lines.front();
    lines.pop_front();
    QPoint start(tr_x(line.x1(),fac), tr_y(line.y1(),fac));
    QPoint end(tr_x(line.x2(),fac), tr_y(line.y2(),fac));
    pen.setColor(line.color());
    painter.setPen(pen);
    painter.save();
    painter.drawLine(start, end);
    painter.restore();
    QColor penColor = QColor(0,0,0);
    pen.setColor(penColor);
    painter.setPen(pen);
  } 
  QColor blue(0,0,255);
  pen.setColor(blue);
  painter.setPen(pen);
  for(int i = 0; i < BOT_MAX; i++)
  {
    painter.save();
    painter.translate((HALF_FIELD_MAXX + state->homePos[i].x)*fac, (HALF_FIELD_MAXY - state->homePos[i].y)*fac);
    //Shashwat bot no. code:
    char c[2];
    c[0] = i + '0';
    c[1] = '\0';
    QString text = QString(c);
    ///
    painter.drawEllipse(rect);
    painter.restore();
    painter.save();
    QPoint midp((HALF_FIELD_MAXX + state->homePos[i].x)*fac, (HALF_FIELD_MAXY - state->homePos[i].y)*fac);
    QPoint endp((HALF_FIELD_MAXX + state->homePos[i].x + BOT_RADIUS*cos(state->homeAngle[i]))*fac,
                (HALF_FIELD_MAXY - state->homePos[i].y - BOT_RADIUS*sin(state->homeAngle[i]))*fac);
    painter.drawText(endp,text);
    painter.drawLine(midp, endp); //draw Direction
    painter.restore();
  }
  QColor yellow(255,0,0);
  pen.setColor(yellow);
  painter.setPen(pen);
  for(int i = 0; i < BOT_MAX; i++)
  {
    painter.save();
    painter.translate((HALF_FIELD_MAXX + state->awayPos[i].x)*fac, (HALF_FIELD_MAXY - state->awayPos[i].y)*fac);
    painter.drawEllipse(rect);
    painter.restore();
    painter.save();
    QPoint midp((HALF_FIELD_MAXX + state->awayPos[i].x)*fac, (HALF_FIELD_MAXY - state->awayPos[i].y)*fac);
    QPoint endp((HALF_FIELD_MAXX + state->awayPos[i].x + BOT_RADIUS * cos(state->awayAngle[i]))*fac,
                (HALF_FIELD_MAXY - state->awayPos[i].y - BOT_RADIUS * sin(state->awayAngle[i]))*fac);
    painter.drawLine(midp, endp); //draw Direction
    painter.restore();
  }
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setPen(palette().dark().color());
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
  timer->start(32);
}
