#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _eofIndex(-1), _nextExpectedIndex(0), _unassembledHeap() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // printf("\n++ enter push_substring ++\n");
    // Above all, update eof information.
    if (eof) {  // latter eof set would override the previous one.
        printf("index = %lu\n", index);  // tag for search: PR
        printf("data.size() = %lu\n", data.size());  // tag for search: PR
        printf("_eofIndex = %lu\n", _eofIndex);  // tag for search: PR
        _eofIndex = index + data.size();
    }

    _unassembledHeap.push(Substring(index, data));

    while (!_unassembledHeap.empty() && _unassembledHeap.top().begin() <= _nextExpectedIndex && _output.remaining_capacity() != 0) {
        Substring ssWithMinorIndex = _unassembledHeap.top(); _unassembledHeap.pop();
        // If there are some usable part of substring, write it.
        if (ssWithMinorIndex.containIndex(_nextExpectedIndex)) {
            string s = ssWithMinorIndex.literal(_nextExpectedIndex - ssWithMinorIndex.begin());
            size_t bytesWritten = _output.write(s);
            _nextExpectedIndex += bytesWritten;
            // If the output is full, write much bit as possible, put the rest
            // substring back to the minor heap.
            if (bytesWritten != s.size()) {
                _unassembledHeap.push(Substring(ssWithMinorIndex.begin()+bytesWritten, ssWithMinorIndex.literal(bytesWritten)));
            }
        }
    }
    if (_nextExpectedIndex >= _eofIndex) {
        _output.end_input();
    }
}

// use klee's algorithm, comput on the fly.
size_t StreamReassembler::unassembled_bytes() const {
    uint64_t n = _unassembledHeap.size();
    vector<pair <uint64_t, bool> > points{2*n};
    uint64_t overlappedSegmentCount = 0;
    size_t result = 0;  // Total range of union of segments.

    // Build an sorted array of 2n beginning and ending of segments.
    for (uint64_t i=0; i<n; i++) {
        auto iit = _unassembledHeap.index(i);
        points[2*i] = make_pair(iit.begin(), false);
        points[2*i+1] = make_pair(iit.end(), true);
    }
    sort(points.begin(), points.end());

    // Examine each interval(2n-1) interval.
    for (uint64_t i=0; i<2*n; i++) {
        if (overlappedSegmentCount) {  // Count > 0 means interval [2n-1, 2n] is in union.
            result += points[i].first - points[i-1].first;
        }
        // is point[i] a ending segment point?
        (points[i].second) ? overlappedSegmentCount-- : overlappedSegmentCount++;
    }
    return result;
}

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
