"""
These are checks for comp1. Feel free to read them. Editing them won't help much,
but if you find a mistake, please let me know!
"""
import check50
import check50.config

check50.config.set_truncate_len(100)

@check50.check()
def exists():
    """matching.cc exists"""
    check50.exists("matching.cc")

@check50.check(exists)
def compiles():
    """matching.cc compiles"""
    check50.run("g++ -c matching.cc -o matching.o").exit(0)
    check50.exists("matching.o")

@check50.check(compiles)
def test1():
    """test1.cpp passes"""
    expect_matches = 0
    expect_score = 0
    link_and_run_test(1, expect_matches, expect_score)

@check50.check(compiles)
def test2():
    """test2.cpp passes"""
    expect_matches = 2
    expect_score = 19
    link_and_run_test(2, expect_matches, expect_score)

@check50.check(compiles)
def no_memory_leak():
    """matching.cc has no memory leaks (test10.cpp)"""
    check50.run("g++ tests/test10.cpp matching.o -o test10 -fsanitize=address -g").exit(0)
    (check50.run("valgrind --leak-check=full ./test10")
     .stdout("All heap blocks were freed -- no leaks are possible")
     .exit(0))

def link_and_run_test(i, expect_matches, expect_score):
    check50.run(f"g++ tests/test{i}.cpp matching.o -o test{i}").exit(0)
    (check50.run(f"./test{i}")
     .stdout(f"Total matches: {expect_matches}\nTotal match score: {expect_score}\n")
     .exit(0))