% PSLL Testing Strategy
% Paul Nickerson

Fuzz-testing
-------------

To test the Pool-using Singly-Linked List in parts 1 and 2, I used the fuzzer described in the SSLL Testing Strategy. Particular attention was paid to writing fuzzer operations which would insert and remove items frequently in an effort to corrupt the free list.