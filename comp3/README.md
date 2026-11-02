# Instructions

1. Make the `noDormMatch` setting work, in `inputs.cc` and `compatibility.cc`.
2. Report a bug or enhancement of your own, and fix it through an issue, a
   branch and a pull request.

## Specifications

You are getting closer to the end!

You've implemented the matching part of the algorithm. You've implemented the
scoring part of the algorithm. Now you'll work in the whole program: this
folder is a copy of the real Datamatch algorithm's C++ code. The one exception
is the matching algorithm, which you wrote in comp1: it comes precompiled as
`matching.o`.

### How the algorithm is laid out

`main.cc` runs the algorithm one school at a time (remember, Datamatch is not
just for Harvard students!), in five stages:

| Stage | File | What it does |
|---|---|---|
| 1 | `inputs.cc` | Reads the input files and builds a `User` for each person (`create_user_from_json`) |
| 2 | `crush_roulette.cc` | Reads crushes and "ships" (people submitting two others as a pair) |
| 3 | `score_matrix.cc` | Scores every pair of users |
| 4 | `match_rounds.cc` | Turns scores into matches, using `perform_matching` from `matching.o` |
| 5 | `output.cc` | Writes `data/results/matches_Harvard.json` |

The math for a single pair lives in:
- `compatibility.cc`: who can be matched at all (`check_compatability`). This is
  the real version of the function you wrote in comp2.
- `scoring.cc`: a pair's score, which is its survey points plus its profile points
- `factors/`: one file per profile factor, listed in `factors/registry.cc`. Like
  in comp2, every score is in answer points.

Shared constants are in `config.h`, and the `User` struct is in `user.h`.

You don't need to read all of it. Start from `main.cc`, which lists the
stages in order, and open a stage's file when you need it.

### Dorm Matches

Users can set `noDormMatch` on their private profile. If it's `true`, they
should never be matched with someone who lives in the same dorm. Right now the
algorithm ignores it. Implementing it takes two changes:

1. **Read it.** In `inputs.cc`, `create_user_from_json` builds each `User` from
   the input JSON. Read `noDormMatch` into `User::no_house_matches`. Not every
   user has it, so check that it exists first (look at how `dorm` is read just
   above the TODO).
2. **Enforce it.** In `compatibility.cc`, `check_compatability` decides whether
   two users can be matched. If either of them set `noDormMatch` and they live
   in the same dorm, return `false`. A dorm of `""` (not given) or `"Other"`
   doesn't count as the same dorm.

Both TODOs are marked in the code. If you only do one of them, the first check
still fails: the setting either never reaches the `User`, or nothing uses it.

### Reporting your own bug

> Run your tests to check your work now. Save this step for last. Only proceed when you feel you have completed the rest. Make sure you've committed and pushed your changes.

Part of working with a team on such a big project is spotting and fixing bugs. 
But, we also like to keep track of possible planned enhancements, or 
suggestions for documentation changes, etc. You can call these problems 
"issues."

We prefer using Github Issues to manage our project's issues, since it's built 
right into the version control system we use.

#### Creating an Issue

For this part, you will find your own bug in the code (or perhaps, if you can't, 
find one, consider a possible enhancement to make to our algorithm). This is
the real algorithm's code, so a bug you find here is probably a real bug too.
Tell us about it!
On your forked repo's Github page, go to the Issues tab and select New Issue.

Create a subject title for your issue, and leave a description good enough so 
that anyone who skims your code can have a somewhat detailed understanding of 
the bug or enhancement. Sometimes, for bugs, its useful to showcase the steps 
to reproduce.

You'll notice some features on the right hand side of the screen, such as 
"Assignees," "Labels," "Projects," and "Milestone." Assign yourself this 
issue (so it's clear you're going to take up the task of working on it) 
and give it a proper label. You can always change these later.

Create your issue.

#### Creating a branch

Afterwards, you'll notice that there is a number next to the title of your 
issue. This is our issue number.

Go to Github Desktop and create a new branch, named the way the algorithm
repository names them. If your issue is a bugfix, title it
`fix/<issue number>/<1-3 word description>`. If your issue is a feature
request, title it `feat/<issue number>/<1-3 word description>`.

For instance, if my bug was about fixing an off by one error in a `for` loop,
you might name the branch `fix/123/loop-bound`. If you were to fix a bug
surrounding dorm matches, you might call it `fix/123/dorm-match`.

#### Creating a pull request

All of your work now lives on this branch. Go ahead and fix your bug or make
your enhancement.

When finished, commit and push your changes. Then, preview your pull request and
then create a pull request on the website. Make sure you're merging into your
`main` branch, and not `ivanharvard`'s `main` branch.

Give it a descriptive title of what happened after the changes were made, 
perhaps something like "Dorm matches correctly function." Now, in the 
description of the pull request, you can write certain keywords to let Github
know this pull request is related to an issue.

If, at the first line of the description you specifically say "Closes #123." 
then Github will close issue #123 if your pull request gets successfully 
merged. Finish creating your pull request.

Normally, this is where you'd stop, and in the real world, I would go in, review
your changes. You will simulate this by simply merging and closing the 
pull request, which will subsequently close the related issue.

Once you're done, your changes should live on the `main` branch. Switch over to
it and check the history to confirm this. Success!

# Checking your work

You can check if your dorm match implementation works below. Depending on
the scale of your bugfix in the second part of this assignment, these checks
may no longer apply to you once you make those changes.

To compile and run on the test data, run
```bash
make run
```
which compiles and then runs
```bash
./main data/FIREBASE_EXPORT.json data/sims data/sims data/sims data/sims
```
The real algorithm takes four similarity folders (answer choices, bios, short
answers, Spotify); here they're all in `data/sims`. The matches are written to
`data/results/matches_Harvard.json`.

To test your work using the autograder, run
```bash
check50 ivanharvard/datamatch-algo-comp/main/comp3 --local
```