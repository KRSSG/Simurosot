#include "stdafx.h"
#include <QtWidgets\qapplication.h>
#include "qtDebuggerMain.h"
#include "main_widget.h"
#include "../Core/beliefState.h"
QtDebugger::QtDebugger(QApplication *app, Strategy::BeliefState *state_): state(state_)
{
  a = app;
}
QtDebugger::~QtDebugger()
{
}
void QtDebugger::run()
{
    MainWidget w(state);
    w.show();
    a->exec();
}

