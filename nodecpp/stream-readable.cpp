#include "stream-readable.h"
#include "process.h"
#include "error.h"

namespace nodecpp {
  
  Readable::Readable() : Readable(json::object()) {}

  Readable::Readable(const json& options) {
    _readableState = ReadableState(options, this);
  }

  bool Readable::push(const Buffer& chunk) {
    return readableAddChunk(_readableState, chunk, false);
  }

  bool Readable::push() {
    return push(Buffer());
  }

  bool Readable::push(const string& str, const string& encoding /*= "utf8"*/) {
    return push(Buffer(str, encoding));
  }

  bool Readable::unshift(const Buffer& chunk) {
    return readableAddChunk(_readableState, chunk, true);
  }

  bool Readable::isPaused() {
    return (_readableState.flowing == false);
  }

  Buffer Readable::read(int n /*= -1*/) {
    auto& state = _readableState;
    auto nOrig = n;
    if (n != 0) state.emittedReadable = false;

    if (n == 0 &&
      state.needReadable &&
      (state.length >= state.highWaterMark || state.ended)) {
      if (state.length == 0 && state.ended) {
        endReadable();
      }
      else {
        emitReadable();
      }
      return Buffer();
    }

    n = howMuchToRead(n, state);

    // if we've ended, and we're now clear, then finish it up.
    if (n == 0 && state.ended) {
      if (state.length == 0)
        endReadable();
      return Buffer();
    }

    // if we need a readable event, then we need to do some reading.
    bool doRead = state.needReadable;

    // if we currently have less than the highWaterMark, then also read some
    if (state.length == 0 || state.length - n < state.highWaterMark) {
      doRead = true;
    }

    // however, if we've ended, then there's no point, and if we're already
    // reading, then it's unnecessary.
    if (state.ended || state.reading) {
      doRead = false;
    }
    else if (doRead) {
      state.reading = true;
      state.sync = true;
      // if the length is currently zero, then we *need* a readable event.
      if (state.length == 0)
        state.needReadable = true;
      // call internal read method
      // this._read(state.highWaterMark);
      state.sync = false;
      // If _read pushed data synchronously, then `reading` will be false,
      // and we need to re-evaluate how much data we can return to the user.
      if (!state.reading)
        n = howMuchToRead(nOrig, state);
    }

    Buffer ret;
    if (n > 0)
      ret = fromList(n, state);

    if (ret.size() == 0) {
      state.needReadable = true;
      n = 0;
    }
    else {
      state.length -= n;
    }

    if (state.length == 0) {
      // If we have nothing in the buffer, then we want to know
      // as soon as we *do* get something into the buffer.
      if (!state.ended)
        state.needReadable = true;

      // If we tried to read() past the EOF, then emit end on the next tick.
      if (nOrig != n && state.ended)
        endReadable();
    }

    if (ret.size() != 0)
      emit<const Buffer&>("data", ret);

    return ret;
  }

  void Readable::on(const string& type, function<void()> listener) {
    Stream::on(type, listener);
  }

  bool Readable::readableAddChunk(ReadableState& state, const Buffer& chunk, bool addToFront) {
    if (chunk.size() == 0) {
      state.reading = false;
      onEofChunk(state);
    }
    else {
      if (state.ended && !addToFront) {
        emit<const Error&>("error", Error("stream.push() after EOF"));
      }
      else if (state.endEmitted && addToFront) {
        emit<const Error&>("error", Error("stream.unshift() after end event"));
      }
      else {
        if (!addToFront) state.reading = false;
        // if we want the data now, just emit it.
        if (state.flowing && state.length == 0 && !state.sync) {
          emit<const Buffer&>("data", chunk);
          read(0);
        }
        else {
          // update the buffer info.
          state.length += chunk.length();
          if (addToFront)
            state.buffer.insert(state.buffer.begin(), chunk);
          else
            state.buffer.emplace_back(chunk);

          if (state.needReadable)
            emitReadable();
        }
        maybeReadMore(state);
      }
    }
    return needMoreData(state);
  }

  void Readable::onEofChunk(ReadableState& state) {
    if (state.ended) return;
    state.ended = true;

    // emit 'readable' now to make sure it gets picked up.
    emitReadable();
  }

  void Readable::emitReadable() {
    auto& state = _readableState;
    state.needReadable = false;
    if (!state.emittedReadable) {
      state.emittedReadable = true;
      if (state.sync) {
        process.nextTick([this]() {
          this->emitReadable_();
        });
      }
      else {
        emitReadable_();
      }
    }
  }

  void Readable::emitReadable_() {
    emit("readable");
    flow();
  }

  void Readable::flow() {
    while (_readableState.flowing && read().size() > 0);
  }

  void Readable::maybeReadMore(ReadableState& state) {
    if (!state.readingMore) {
      state.readingMore = true;
      process.nextTick([this]() {
        this->maybeReadMore_(_readableState);
      });
    }
  }

  void Readable::maybeReadMore_(ReadableState& state) {
    auto len = state.length;
    while (!state.reading && !state.flowing && !state.ended &&
      state.length < state.highWaterMark) {
      read(0);
      if (len == state.length)
        // didn't get any data, stop spinning.
        break;
      else
        len = state.length;
    }
    state.readingMore = false;
  }

  void Readable::endReadable() {
    auto& state = _readableState;

    // If we get here before consuming all the bytes, then that is a
    // bug in node.  Should never happen.
    if (state.length > 0)
      throw new Error("(\"endReadable()\" called on non-empty stream");

    if (!state.endEmitted) {
      state.ended = true;
      process.nextTick([this]() {
        this->endReadableNT(_readableState);
      });
    }
  }

  void Readable::endReadableNT(ReadableState& state) {
    // Check that we didn't get one last unshift.
    if (!state.endEmitted && state.length == 0) {
      state.endEmitted = true;
      readable = false;
      emit("end");
    }
  }

  uint32_t Readable::howMuchToRead(int n, ReadableState& state) {
    if (state.length == 0 && state.ended)
      return 0;
    if (n == -1) {
      // Only flow one buffer at a time
      if (state.flowing && state.length)
        return state.buffer.begin()->length();
      else
        return state.length;
    }
    if (n <= static_cast<int>(state.length))
      return n;
    // Don't have enough
    if (!state.ended) {
      state.needReadable = true;
      return 0;
    }
    return state.length;
  }

  bool Readable::needMoreData(ReadableState& state) {
    return !state.ended &&
      (state.needReadable ||
        state.length < state.highWaterMark ||
        state.length == 0);
  }

  void Readable::resume() {
    auto& state = _readableState;
    if (!state.flowing) {
      state.flowing = true;
      if (!state.resumeScheduled) {
        state.resumeScheduled = true;
        process.nextTick([this]() {
          resume_(_readableState);
        });
      }
    }
  }

  void Readable::resume_(ReadableState& state) {
    if (!state.reading) {
      read(0);
    }

    state.resumeScheduled = false;
    state.awaitDrain = 0;
    emit("resume");
    flow();
    if (state.flowing && !state.reading) {
      read(0);
    }
  }

  Buffer Readable::fromList(int n, ReadableState& state) {
    Buffer ret;
    if (n >= static_cast<int>(state.length)) {
      if (state.buffer.size() == 1) {
        ret = state.buffer.front();
      }
      else {
        ret = Buffer::concat(state.buffer, state.length);
      }
      state.buffer.clear();
    }
    else {
      // read part of list
      ret = fromListPartial(n, state.buffer);
    }
    return ret;
  }

  nodecpp::Buffer Readable::fromListPartial(int n, std::list<Buffer>& list) {
    Buffer ret;
    if (list.size() > 0) {
      size_t headLen = list.front().size();
      if (n < static_cast<int>(headLen)) {
        ret = list.front().slice(0, n);
        list.front() = list.front().slice(n);
      }
      else if (n == static_cast<int>(headLen)) {
        ret = list.front();
        list.pop_front();
      }
    }

    return ret;
  }

}