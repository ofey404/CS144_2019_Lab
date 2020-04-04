#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    if (!_synced) {
        if (seg.header().syn) {
            _synced = true;
            _isn = seg.header().seqno;
        } else {
            return false;
        }
    }

    if (inWindow(seg)) {
        uint64_t absSeqno = unwrap(seg.header().seqno, _isn, _checkpoint);
        uint64_t stringIndex = absSeqno = 0 ? 0 : absSeqno - 1;
        pushAck(seg);
        printf("absSeqno = %lu\n", absSeqno);
        printf("static_cast<string>(seg.payload().str()) = %s\n", static_cast<string>(seg.payload().str()).c_str());
        printf("seg.header().fin = %d\n", seg.header().fin);
        _reassembler.push_substring(static_cast<string>(seg.payload().str()), stringIndex, seg.header().fin);
        _checkpoint = absSeqno;
        printf("_reassembler.stream_out().input_ended() = %d\n", _reassembler.stream_out().input_ended());
        return true;
    } else {
        return false;
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_synced) {
        return {};
    } else {
        return wrap(_absAckno, _isn);
    }
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }

WrappingInt32 TCPReceiver::pushAck(const TCPSegment &seg) {
    _unresolvedIntervals.push(Interval(seg.header().seqno, seg.length_in_sequence_space(), _isn, _checkpoint));
    while (!_unresolvedIntervals.empty() && _unresolvedIntervals.top().begin <= _absAckno) {
        Interval left = _unresolvedIntervals.top(); _unresolvedIntervals.pop();
        _absAckno = left.end();
    }
    return wrap(_absAckno, _isn);
}

bool TCPReceiver::inWindow(const TCPSegment &seg) {
    if (!ackno().has_value()) return false;

    uint64_t begin = unwrap(seg.header().seqno, _isn, _checkpoint);
    size_t length = seg.length_in_sequence_space();
    return inWindow(begin, length);
}

bool TCPReceiver::inWindow(const uint64_t &begin, const size_t &length) {
    if (!ackno().has_value()) return false;
    if (begin < _absAckno+window_size() && _absAckno < begin+length) {
        return true;
    } else {
        return false;
    }
}
