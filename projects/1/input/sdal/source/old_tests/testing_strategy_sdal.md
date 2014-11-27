% SDAL Testing Strategy
% Paul Nickerson

Fuzz-testing
-------------

To test the Simple Dynamic Array-based List in parts 1 and 2, I used the fuzzer described in the SSLL Testing Strategy. Particular attention was paid to writing fuzzer oerations which would force the list to grow and shrink so that the reallocation code paths would be exercised.