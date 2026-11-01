# ENABLE_CHECK50_ASSERT = 1
import shutil
from pathlib import Path

import check50
import check50.c
import check50.config
import check50.internal

check50.config.set_truncate_len(50)

def use_latest(*paths):
    """
    Copy these files from the downloaded checks into the copy being checked,
    so the newest tests and answers are used even if a fork isn't synced yet.
    """
    check_dir = Path(check50.internal.check_dir)
    for path in paths:
        src, dst = check_dir / path, Path(path)
        if dst.exists() and src.resolve() == dst.resolve():
            continue
        if src.is_dir():
            shutil.copytree(src, dst, dirs_exist_ok=True)
        else:
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(src, dst)

def run_test(i):
    """Runs `make i` and reports the FAIL lines the test printed."""
    proc = check50.run(f"make {i}")
    output = proc.stdout(timeout=60)  # waits for make to finish
    code = proc.exitcode

    reasons = [line[len("FAIL: "):] for line in output.splitlines()
               if line.startswith("FAIL: ")]
    if reasons:
        rationale, help = reasons[0], "\n".join(reasons[1:]) or None
    else:
        # No FAIL lines usually means the test didn't compile or crashed.
        rationale, help = f"test{i} failed with exit code {code}", output.strip() or None
    assert code == 0, check50.Failure(rationale, help=help)

@check50.check()
def exists():
    """scoring.cc exists"""
    check50.exists("scoring.cc")
    use_latest("tests", "answers.o", "Makefile")

@check50.check(exists)
def compiles():
    """scoring.cc compiles"""
    check50.run("make scoring").exit(0)
    check50.exists("scoring.o")

@check50.check(compiles)
def same_user():
    """check_compatibility returns false for the same user, but not for everyone"""
    run_test(1)

@check50.check(compiles)
def different_colleges():
    """check_compatibility returns false only for users attending different colleges"""
    run_test(2)

@check50.check(compiles)
def match_type():
    """check_compatibility respects love/friendship preferences"""
    run_test(3)

@check50.check(compiles)
def mutual_crushes():
    """check_compatibility returns true for mutual crushes"""
    run_test(4)
    
@check50.check(compiles)
def class_year_gaps():
    """check_compatibility respects class year gaps"""
    run_test(5)

@check50.check(compiles)
def age_preferences():
    """check_compatibility respects age preferences"""
    run_test(6)

@check50.check(compiles)
def same_house_preferences():
    """check_compatibility respects same-house preferences"""
    run_test(7)

@check50.check(compiles)
def blocked_house_preferences():
    """check_compatibility respects blocked house preferences"""
    run_test(8)

@check50.check(compiles)
def bonus():
    """bonus function makes a meaningful contribution to score calculation"""
    # the bonus function by default only adds 0 to the score, so if the score
    # changes, then we can reasonably conclude that the bonus function has
    # probably been implemented.

    default_score = float(check50.run("make answers-9").stdout().strip("Score: "))
    your_score    = float(check50.run("make 9").stdout().strip("Score: "))
    assert abs(default_score - your_score) > 1e-3, "did you implement the bonus function?"