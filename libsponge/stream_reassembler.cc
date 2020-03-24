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
size_t StreamReassembler::unassembled_bytes() const {
    uint64_t n = _indexToUnassembledBytes.size();
    vector<pair <uint64_t, bool> > points{2*n};

    for (uint64_t i=0; i<n; i++) {
        auto iit = _indexToUnassembledBytes.begin();
        points[2*i] = make_pair(iit->first, false);
        points[2*i+1] = make_pair(iit->first + iit->second.size(), true);
    }
    sort(points.begin(), points.end());
    size_t result = 0;
    uint64_t overlappedSegmentCount = 0;

    for (uint64_t i=0; i<2*n; i++) {
        if (overlappedSegmentCount) {
            result += points[i].first - points[i-1].first;
        }
        // is point[i] a ending segment point?
        (points[i].second) ? overlappedSegmentCount-- : overlappedSegmentCount++;
    }
    return result;
}

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
