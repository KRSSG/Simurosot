#include "../common/include/thread.h"
#include <QtWidgets\qapplication.h>
namespace Strategy {
  class BeliefState;
}
class QtDebugger
{
public:
  QtDebugger(QApplication *app, Strategy::BeliefState *state);
  ~QtDebugger();
  QApplication* a;
  Strategy::BeliefState *state;
  void run();
};