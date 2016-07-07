#include "child-process.h"
#include "process-wrap.h"
namespace nodecpp {

  void ChildProcess::spawn() {
    auto child = new ProcessWrap();
    child->spawn();
  }

}