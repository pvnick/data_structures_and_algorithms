% CDAL Testing Strategy
% Paul Nickerson

Fuzz-testing
-------------

To test the Chained Dynamic Array-based List in parts 1 and 2, I used the fuzzer described in the SSLL Testing Strategy. Particular attention was paid to writing fuzzer operations which grew the list and then targeted items in the middle of the list in an effort to disrupt the CDAL mechanism that traverses across the chain and down the item array. In addition, copy operations were considered to be a potential source of bugs, so those code paths were targeted as well.