#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _eofIndex(-1), _unassembledBytesCount(0), _nextExpectedIndex(0), _indexToUnassembledBytes(), _unassembledIndices() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {  // latter eof set would override the previous one.
        _eofIndex = index + data.size();
    }

    _unassembledIndices.push(index);
    _indexToUnassembledBytes[index] = data;
    _unassembledBytesCount += data.size();
    uint64_t minorIndex = _unassembledIndices.top();

    while (minorIndex <= _nextExpectedIndex && !_unassembledIndices.empty()) {
        string minorIndexBytes = _indexToUnassembledBytes[minorIndex];
        if (minorIndex + minorIndexBytes.size() > _nextExpectedIndex) {
            _output.write(minorIndexBytes.substr(_nextExpectedIndex-minorIndex));
            _nextExpectedIndex += minorIndexBytes.size() - (_nextExpectedIndex - minorIndex);
        }
        _unassembledBytesCount -= minorIndexBytes.size();
        _indexToUnassembledBytes.erase(minorIndex);
        _unassembledIndices.pop();
        minorIndex = _unassembledIndices.top();  // update minorIndex for possible next loop
    }

    if (_nextExpectedIndex == _eofIndex) { // if reach eof
        _output.end_input();
    }

}

// use klee's algorithm, comput on the fly.
size_t StreamReassembler::unassembled_bytes() const { return _unassembledBytesCount; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
