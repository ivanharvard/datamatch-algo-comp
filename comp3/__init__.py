# ENABLE_CHECK50_ASSERT = 1
import check50
import check50.internal
import json
import shutil
from pathlib import Path

def pair_key(a: str, b: str) -> str:
    a, b = sorted([a, b])
    return f"{a}-{b}"

def use_latest(*paths):
    """
    Copy these files from the downloaded checks into the copy being checked,
    so the newest test data is used even if a fork isn't synced yet.
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

def load_matches():
    p = Path("data/results/matches_Harvard.json")
    assert p.exists(), "data/results/matches_Harvard.json not found"
    return json.loads(p.read_text())

@check50.check()
def exists():
    """inputs.cc and compatibility.cc exist"""
    check50.exists("inputs.cc", "compatibility.cc", "main.cc")
    use_latest("data", "matching.o", "Makefile")

@check50.check(exists, timeout=200)
def compiles():
    """main compiles"""
    # Most files include the large json.hpp, so compiling can take a while.
    check50.run("make main").exit(0, timeout=180)

@check50.check(compiles)
def binary_present():
    """main binary was produced"""
    check50.exists("main")

@check50.check(compiles)
def runs():
    """program runs successfully"""
    check50.run("./main data/FIREBASE_EXPORT.json data/sims data/sims data/sims data/sims").exit(0, timeout=60)

@check50.check(runs)
def u1_u2_cannot_date_if_same_dorm_and_noDormMatch():
    """u1 and u2 cannot date when same dorm and noDormMatch=true"""
    matches = load_matches()
    key = pair_key("u1", "u2")

    if key not in matches:
        # Ideal outcome: rule blocked the match entirely.
        return

    # If present anyway, ensure canDate is false.
    entry = matches[key]
    can_date = entry.get("canDate")
    assert can_date == False, f"{key} present with canDate=true (should not be allowed)"

@check50.check(runs)
def u2_u4_still_match_in_different_dorms():
    """u2 and u4 still match when they live in different dorms"""
    matches = load_matches()
    key = pair_key("u2", "u4")
    assert key in matches, check50.Failure(
        f"{key} is missing from the matches",
        help="u2 (Adams) and u4 (Mather) live in different dorms, so noDormMatch "
             "shouldn't stop them from matching.",
    )

@check50.check(runs)
def u3_u5_still_match_without_noDormMatch():
    """u3 and u5 still match in the same dorm when neither set noDormMatch"""
    matches = load_matches()
    key = pair_key("u3", "u5")
    assert key in matches, check50.Failure(
        f"{key} is missing from the matches",
        help="u3 and u5 both live in Lowell, but neither set noDormMatch, so "
             "they should still be able to match.",
    )
