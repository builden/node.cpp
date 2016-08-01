#include "child-process.h"
#include "process-wrap.h"
namespace nodecpp {
  ChildProcess::ChildProcess() {
    handle_ = new ProcessWrap();
  }

  int ChildProcess::spawn(json options) {
    spawnfile_ = options["file"].get<string>();
    spawnargs_ = options["args"].get<svec_t>();

    int err = handle_->spawn(options);
    return err;
  }

}