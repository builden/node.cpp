#pragma once
#include "stream.h"
#include "buffer.h"

namespace nodecpp {
  class ReadableState {
  public:
    bool flowing = false;
    bool ended = false;
    bool endEmitted = false;
    bool reading = false;

    bool sync = true;
    bool needReadable = false;
    bool emittedReadable = false;
    bool readableListening = false;
    bool resumeScheduled = false;
  };

  class Readable : public Stream {
  public:
    Readable();
    Readable(const json& options);
    void push(const string& str, const string& encoding = "utf8");
    void push(const Buffer& buf);
    void push();

    ReadableState _readableState;
  };
}