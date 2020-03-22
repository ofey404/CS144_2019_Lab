#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity): _buffer(string()), _capacity(capacity) {
    _inputEnded = false;
    _readCount = 0;
    _writeCount = 0;
}

size_t ByteStream::write(const string &data) {
    int freePlaceLength = _capacity - _buffer.size();
    int dataLength = data.size();
    if (dataLength > freePlaceLength) {
        _buffer += data.substr(0, freePlaceLength);
        _writeCount += freePlaceLength;
        return freePlaceLength;
    } else {
        _buffer += data;
        _writeCount += dataLength;
        return dataLength;
    }
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if (len > _buffer.size()) {
        return _buffer;
    } else {
        return _buffer.substr(0, len);
    }
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > _buffer.size()) {
        _readCount += _buffer.size();
        _buffer.clear();
    } else {
        _readCount += len;
        _buffer = _buffer.substr(len);  // probably cause problem
    }
}

void ByteStream::end_input() { _inputEnded = true; }

bool ByteStream::input_ended() const { return _inputEnded; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return _inputEnded && _buffer.size() == 0; }

size_t ByteStream::bytes_written() const { return _writeCount; }

size_t ByteStream::bytes_read() const { return _readCount; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
