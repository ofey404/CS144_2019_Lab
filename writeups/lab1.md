Lab 1 Writeup
=============

My name: Ofey Chan

My SUNet ID: [your sunetid here]

This lab took me about 3 hours to do.

Program Structure and Design of the StreamReassembler:

**Algorithm of `push_substring()`**:

```python
keep track of the next expected index.
pick the smallest index of unassembled bytes.

while the index is not larger than expected:
    merge this byte(or discard it)
    modify the next expected index
    check the next smallest index of uassembled bytes.
```

**How to implement eof**: Keep track of the eof index.

In the begin of `push_substring()`, update the eof index(the latter would overwrite the former, for the sake of easier implementation).

**The implementation of `unassembled_byte()` counting**: Use klee's algorithm, compute on the fly.

One reason of choosing to compute on the fly, is that I missunderstood the `unassembled_byte()` at first, and when I realise this mistake, the basic chunk of code is done. I don't want to bother myself to change the previous written code.

Another reason is that I guess the `unassembled_byte()` function won't be called very often, so it's ok to pile the overhead(O(nLogn), n is the unassembled chunk number) in it. Otherwise this overhead won't be gone, but spread in the operations maintaining a count of unassembled bytes.

Data structure:
- A map to keep track the unassembled index-bytes pair.
- A minor heap to get the minimum index.
- A helper vector to count the unassembled bytes.

Implementation Challenges:

The addition and subtraction of indices, I always afraid to miss or missadd a one.

Didn't understand the behavior of interface at first. Correct it according to the tests.

Remaining Bugs:
Memory consumption don't limited properly, for the unassembled bytes can grow beyond the `capacity` limit.

- Optional: I had unexpected difficulty with: Understanding the behavior of interface.

- Optional: I think you could make this lab better by: change the comment in the interface of `unassembled_bytes()` into:
    - // The number of bytes in the substrings stored but not yet reassembled, overlapped segments only count once.

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
