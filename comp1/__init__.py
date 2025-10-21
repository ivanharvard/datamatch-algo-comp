"""
These are checks for comp1. Feel free to read them. Editing them won't help much,
but if you find a mistake, please let me know!
"""
import check50

@check50.check()
def exists():
    """matching.cc exists"""
    check50.exists("matching.cc")

@check50.check(exists)
def compiles_obj():
    """matching.cc compiles"""
    check50.run("g++ -c matching.cc -o matching.o").exit(0)
    check50.exists("matching.o")

@check50.check(compiles_obj)
def test1():
    """test1.cpp passes"""
    check50.run("g++ -std=c++17 -I. tests/test1.cpp answers.o -o test1").exit(0)
    check50.exists("test1")
    check50.run("./test1").exit(0)