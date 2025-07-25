#pragma once

#include <stdint.h>

template <typename T, uint16_t _size>
class CircularBuffer
{
public:
  CircularBuffer() {}

  bool put(const T *buf, uint16_t len)
  {
    uint16_t available = _available();

    if (available < len) {
      return false;
    }

    if (len == available) {
      d_full = true;
    }

    if (d_head + len >= _size) {
      uint16_t cycle = _size - d_head;
      for (uint16_t i = 0; i < cycle; ++i) {
        d_data[d_head + i] = buf[i];
      }
      for (uint16_t i = 0; i < len - cycle; ++i) {
        d_data[i] = buf[i + cycle];
      }
      d_head = len - cycle;
    } else {
      for (uint16_t i = 0; i < len; ++i) {
        d_data[d_head + i] = buf[i];
      }
      d_head += len;
    }

    return true;
  }
  bool get(uint16_t num, T *buf)
  {
    if (_used() < num) {
      return false;
    }

    if (num > 0) {
      d_full = false;
    }

    if (d_tail + num >= _size) {
      uint16_t cycle = _size - d_tail;

      for (uint16_t i = 0; i < cycle; ++i) {
        buf[i] = d_data[d_tail + i];
      }
      for (uint16_t i = 0; i < num - cycle; ++i) {
        buf[i + cycle] = d_data[i];
      }
      d_tail = num - cycle;
    } else {
      for (uint16_t i = 0; i < num; ++i) {
        buf[i] = d_data[d_tail + i];
      }
      d_tail += num;
    }

    return true;
  }
  bool peek(uint16_t num, T *buf) const
  {
    if (_used() < num) {
      return false;
    }

    if (d_tail + num >= _size) {
      uint16_t cycle = _size - d_tail;

      for (uint16_t i = 0; i < cycle; ++i) {
        buf[i] = d_data[d_tail + i];
      }
      for (uint16_t i = 0; i < num - cycle; ++i) {
        buf[i + cycle] = d_data[i];
      }
    } else {
      for (uint16_t i = 0; i < num; ++i) {
        buf[i] = d_data[d_tail + i];
      }
    }

    return true;
  }

  uint16_t find(const T &elem) const
  {
    if (empty()) {
      return -1;
    }

    uint16_t used = _used();
    uint16_t i    = 0;
    if (d_tail + used > _size) {
      uint16_t cycle = _size - d_tail;

      while (i < cycle && d_data[d_tail + i] != elem) {
        ++i;
      }

      if (d_data[d_tail + i] != elem) {
        uint16_t j = 0;
        while (j < used - cycle && d_data[j] != elem) {
          ++j;
        }
        i += j;
      }
    } else {
      while (i < used && d_data[d_tail + i] != elem) {
        ++i;
      }
    }

    if (i == used) {
      i = -1;
    }

    return i;
  }

  bool eraseFirst(size_t num)
  {
    if (_used() < num) {
      return false;
    }

    if (num > 0) {
      d_full = false;
    }

    d_tail += num;

    if (d_tail >= _size) {
      d_tail -= _size;
    }

    return true;
  }

  uint16_t size() const { return _size; }
  uint16_t available() const { return _available(); }
  uint16_t used() const { return _used(); }
  bool empty() const { return used() == 0; }
  bool full() const { return d_full; }

  void reset()
  {
    d_tail = d_head = 0;
    d_full          = 0;
  }

protected:
  inline uint16_t _available() const
  {
    if (d_full) {
      return 0;
    }

    if (d_tail > d_head) {
      return d_tail - d_head;
    } else {
      return _size - d_head + d_tail;
    }
  }
  inline uint16_t _used() const { return _size - _available(); }

protected:
  T d_data[_size];
  bool d_full     = false;
  uint16_t d_head = 0;
  uint16_t d_tail = 0;
};