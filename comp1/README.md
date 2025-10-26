# Instructions

Complete two functions in `matching.cc`: `try_propose` and `make_matches`.

If you see something like:
```
:( test6.cpp passes
    expected: "Total matches: 2\nTotal match score: 19\n"
    actual:   "test6: matching.cc:210: void run(int, int**, int**..."
    checked: output == solution
    where solution = 'Total matches: 2\nTotal match score: 19\n', output = 'test6: matching.cc:210: void run(int, int**, int**, bool**, int*, int, std::set<int>*): Assertion `scores[i][j] < 0 && "Did you implement try_propose and make_matches yet?"\' failed.\n'
:( test7.cpp passes
    expected: "Total matches: 499\nTotal match score: 39013\n"
    actual:   "test7: matching.cc:210: void run(int, int**, int**..."
    checked: output == solution
    where solution = 'Total matches: 499\nTotal match score: 39013\n', output = 'test7: matching.cc:210: void run(int, int**, int**, bool**, int*, int, std::set<int>*): Assertion `scores[i][j] < 0 && "Did you implement try_propose and make_matches yet?"\' failed.\n'
```
then you likely haven't completed `try_propose` and `make_matches`.

# Checking your work

You can check an individual test file by running
```
make <test number>
```

You can check what we expect your file to output by running
```
make answers-<test number>
```

For instance, `test1` should automatically pass with the handout code. You can
try this by checking:
```
make 1
```
and comparing that to
```
make answers-1
```
which should lead to the same output

You can check all of the tests, and some useful output, by running
```
check50 ivanharvard/datamatch-algo-comp/main/comp1 --local
```