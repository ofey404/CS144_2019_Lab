#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _eofIndex(-1), _nextExpectedIndex(0), _indexToUnassembledBytes(), _unassembledIndices() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // printf("\n++ enter push_substring ++\n");
    // Above all, update eof information.
    if (eof) {  // latter eof set would override the previous one.
        _eofIndex = index + data.size();
    }

    _unassembledIndices.push(index);
    _indexToUnassembledBytes[index] = data;
    uint64_t minorIndex = _unassembledIndices.top();


    // In every push, merge all possible substring into the sequence.
    while (minorIndex <= _nextExpectedIndex && !_unassembledIndices.empty()) {
        string minorIndexBytes = _indexToUnassembledBytes[minorIndex];
        if (minorIndex + minorIndexBytes.size() > _nextExpectedIndex) {
            if (minorIndexBytes.size() - (_nextExpectedIndex - minorIndex) > _capacity - _output.buffer_size()) {
                break;
            }
            // printf("Output written, %s\n", minorIndexBytes.substr(_nextExpectedIndex-minorIndex).c_str());
            // printf("_output.buffer_size() = %lu\n", _output.buffer_size());
            _output.write(minorIndexBytes.substr(_nextExpectedIndex-minorIndex));
            _nextExpectedIndex += minorIndexBytes.size() - (_nextExpectedIndex - minorIndex);
        }
        // printf("Assemble index %lu\n", minorIndex);
        // printf("%s\n", minorIndexBytes.c_str());
        // printf("_output.byte_written() = %lu\n", _output.bytes_written());
        // printf("_output.peek_output(100) = %s\n", _output.peek_output(100).c_str());
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
    uint64_t overlappedSegmentCount = 0;
    size_t result = 0;  // Total range of union of segments.

    // Build an sorted array of 2n beginning and ending of segments.
    for (uint64_t i=0; i<n; i++) {
        auto iit = _indexToUnassembledBytes.begin();
        points[2*i] = make_pair(iit->first, false);
        points[2*i+1] = make_pair(iit->first + iit->second.size(), true);
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
