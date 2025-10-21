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
def compiles():
    """matching.cc compiles"""
    check50.run("make matching").exit(0) 