#pragma once
#include "stream.h"
#include "buffer.h"

namespace nodecpp {
  class ReadableState {
  public:
    ReadableState() {}
    ReadableState(json options, const Stream* /*stream*/) {
      // the point at which it stops calling _read() to fill the buffer
      // Note: 0 is a valid value, means "don't call _read preemptively ever"
      uint32_t defaultHwm = 16 * 1024;
      highWaterMark = defaultHwm;
      if (options.is_object() && options["highWaterMark"].is_number()) {
        highWaterMark = options["highWaterMark"].get<uint32_t>();
      }
    }

    uint32_t highWaterMark = 0;
    std::list<Buffer> buffer;
    uint32_t length = 0;
    bool flowing = false;
    bool ended = false;
    bool endEmitted = false;
    bool reading = false;

    // a flag to be able to tell if the onwrite cb is called immediately,
    // or on a later tick.  We set this to true at first, because any
    // actions that shouldn't happen until "later" should generally also
    // not happen before the first write call.
    bool sync = true;

    // whenever we return null, then we set a flag to say
    // that we're awaiting a 'readable' event emission.
    bool needReadable = false;
    bool emittedReadable = false;
    bool readableListening = false;
    bool resumeScheduled = false;

    string defaultEncoding = "utf8";

    // when piping, we only care about 'readable' events that happen
    // after read()ing all the bytes and not getting any pushback.
    bool ranOut = false;

    // the number of writers that are awaiting a drain event in .pipe()s
    int awaitDrain = 0;

    // if true, a maybeReadMore has been scheduled
    bool readingMore = false;
  };

  class Readable : public Stream {
  public:
    Readable();
    Readable(const json& options);
    bool push(const string& str, const string& encoding = "utf8");
    bool push(const Buffer& chunk);
    bool push();
    bool unshift(const Buffer& chunk);
    bool isPaused();
    Buffer read(int n = -1);

    template <typename... Args>
    void on(const string& type, function<void(Args...)> listener);
    void on(const string& type, function<void()> listener);

    ReadableState _readableState;

    string className = "Readable";

  private:
    bool readableAddChunk(ReadableState& state, const Buffer& chunk, bool addToFront);
    void onEofChunk(ReadableState& state);
    void emitReadable();
    void emitReadable_();
    void flow();
    void maybeReadMore(ReadableState& state);
    void maybeReadMore_(ReadableState& state);
    void endReadable();
    void endReadableNT(ReadableState& state);
    uint32_t howMuchToRead(int n, ReadableState& state);
    bool needMoreData(ReadableState& state);
    void resume();
    void resume_(ReadableState& state);
    Buffer fromList(int n, ReadableState& state);
    Buffer fromListPartial(int n, std::list<Buffer>& list);
  private:
    bool readable = true;
  };

  template <typename... Args>
  void Readable::on(const string& type, function<void(Args...)> listener) {
    Stream::on(type, listener);
    if (type == "data") {
      // Start flowing on next tick if stream isn't explicitly paused
      // if (_readableState.flowing != false)
        resume();
    }
  }
}
