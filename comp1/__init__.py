# ENABLE_CHECK50_ASSERT = 1
"""
These are checks for comp1. Feel free to read them. Editing them won't help much,
but if you find a mistake, please let me know!
"""
import check50
import check50.config
import check50.c

check50.config.set_truncate_len(50)

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
    solution_matches_test(1)

@check50.check(compiles)
def test2():
    """test2.cpp passes"""
    solution_matches_test(2)

@check50.check(compiles)
def test3():
    """test3.cpp passes"""
    solution_matches_test(3)

@check50.check(compiles)
def test4():
    """test4.cpp passes"""
    solution_matches_test(4)

@check50.check(compiles)
def test5():
    """test5.cpp passes"""
    solution_matches_test(5)

@check50.check(compiles)
def test6():
    """test6.cpp passes"""
    solution_matches_test(6)

@check50.check(compiles)
def test7():
    """test7.cpp passes"""
    solution_matches_test(7)

@check50.check(test7)
def no_memory_leak():
    """matching.cc has no memory leaks"""
    test = "test7" # we only test leaks on this

    check50.run(f"g++ tests/{test}.cpp matching.o -o {test}").exit(0)
    check50.c.valgrind(f"./{test}").stdout(timeout=10)

def solution_matches_test(i):
    check50.run(f"g++ tests/test{i}.cpp answers.o -o test{i}_sol").exit(0)
    check50.run(f"g++ tests/test{i}.cpp matching.o -o test{i}").exit(0)

    solution = check50.run(f"./test{i}_sol").stdout()
    output = check50.run(f"./test{i}").stdout()

    # check50 truncates long output, so explain the common failures in full.
    crashed = "Assertion" in output
    assert not crashed, check50.Failure(
        f"test{i} crashed on an assert in matching.cc",
        help=output.strip(),
    )

    made_no_matches = output.startswith("Total matches: 0\n")
    expected_matches = not solution.startswith("Total matches: 0\n")
    assert not (made_no_matches and expected_matches), check50.Failure(
        "no matches were made",
        help="Have you implemented try_propose and make_matches yet?",
    )

    assert output == solution