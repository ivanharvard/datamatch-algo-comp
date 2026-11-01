# Instructions

Complete two functions in `matching.cc`: `try_propose` and `make_matches`.

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
If you see
```
Success!
```
twice, you're good to go!

You can check all of the tests, and some useful output, by running
```
check50 ivanharvard/datamatch-algo-comp/main/comp1 --local
```

By the way, if you see something like:
```text
:( test5.cpp passes
    no matches were made
    Have you implemented try_propose and make_matches yet?
:( test6.cpp passes
    no matches were made
    Have you implemented try_propose and make_matches yet?
```
then you likely haven't completed `try_propose` and `make_matches`.