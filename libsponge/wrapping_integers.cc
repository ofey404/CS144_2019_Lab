#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn + uint32_t(n);  // This plus operator between type is overloaded.
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    int32_t increment = n - isn;  // could be negative.

    // Generate first test of absolute sequence number.
    // The first test is set in [base, base+(1ul << 32) )
    uint64_t absoluteIncrement = increment >= 0? increment : (1ul << 32) + increment;
    uint64_t base = checkpoint & ~0xffffffffUL;  // Higher 32 bit of checkpoint.
    uint64_t test1 = base + absoluteIncrement;

    // If the range between first test and checkpoint is further than half of 
    // the wrapping interval, the first test would be rejected.
    if ((test1 > checkpoint ? test1 - checkpoint : checkpoint - test1) <= (1ul << 31)) {
        return test1;
    } else if (test1 < checkpoint) {  // If test1 is smaller, the round up could be done safely.
        return test1 + (1ul << 32);
    } else {
        if (base == 0) { // Absolute index can't less than zero, so just let it go.
            return test1;
        } else {
            return test1 - (1ul << 32);
        }
    }
}
