#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

class Substring {
  private:
    std::string _literal;
    uint64_t _begin;

  public:
    Substring(const uint64_t beginIndex, const std::string literalString):  _literal(literalString), _begin(beginIndex) {}

    // copy constructor
    Substring(const Substring &s):  _literal(s.literal()), _begin(s.begin()) {}

    bool operator<(const Substring &s) {
        return _begin > s.begin();
    }

    // Literal from `begin` to end.
    std::string literal(const uint64_t &begin=0) const { return _literal.substr(begin); }

    // Beginning index of substring.
    uint64_t begin() const { return _begin; }

    // First non-substring index.
    uint64_t end() const { return _begin + _literal.size(); }

    // If the range of substring contains given index
    bool containIndex(uint64_t index) const {
        return (begin() <= index && index < end());
    }

    // If two substring overlaps.
    bool isOverlapping(const Substring &s) const {
        return begin() <= s.end() && end() <= s.begin();
    }

    // Merge two overlapped substring.
    // If unfortunately, two substring don't overlap, throw an error.
    const Substring merge(const Substring &s) const {
        if (!isOverlapping(s)) {
            throw "merged substring are not overlapping:\n" + _literal + "\nand" + s.literal() + "\n";
        }
        Substring left{begin() < s.begin() ? *this : s};
        Substring right{begin() < s.begin() ? s : *this};

        if (left.end() >= right.end()) {
            return left;
        } else {
            uint64_t rightBegin = left.end() - right.begin();
            uint64_t rightLength = right.end() - left.end();
            return Substring(left.begin(), left.literal() + right.literal().substr(rightBegin, rightLength));
        }
    }
};

template <class T>
class IterableHeap {
  private:
    std::vector<T> _container;
  public:
    IterableHeap(): _container() { make_heap(_container.begin(), _container.end()); }
    void push(T element) { _container.push_back(element); push_heap(_container.begin(), _container.end()); }
    const T& top() const { return _container.front(); }
    void pop() { pop_heap(_container.begin(), _container.end()); _container.pop_back(); }
    bool empty() const { return _container.empty(); }
    uint64_t size() const { return _container.size(); }
    const T& index(uint64_t x) const { return _container[x]; }
};

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes
    size_t _eofIndex; 
    uint64_t _nextExpectedIndex;
    IterableHeap<Substring> _unassembledHeap;  // Minor heap to contain unassembled substring.


  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receives a substring and writes any newly contiguous bytes into the stream.
    //!
    //! If accepting all the data would overflow the `capacity` of this
    //! `StreamReassembler`, then only the part of the data that fits will be
    //! accepted. If the substring is only partially accepted, then the `eof`
    //! will be disregarded.
    //!
    //! \param data the string being added
    //! \param index the index of the first byte in `data`
    //! \param eof whether or not this segment ends with the end of the stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been submitted twice, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
