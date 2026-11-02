# Instructions

1. Complete `check_compatibility` in `compatibility.cc`.
2. Write a new profile factor in `factors/my_factor.cc`, and add it to the list
   in `factors/registry.cc`.

## How this fits together

The files here are laid out the same way as the real algorithm's scoring code:

| File | What it does |
|---|---|
| `compatibility.cc` | Decides who can be matched at all (`check_compatibility`) |
| `scoring.cc` | Computes a pair's score: `survey_points` + `profile_points` (provided) |
| `factors/` | One file per profile factor; `factors/registry.cc` lists them all |
| `user.hpp` | The `User` struct |

## Specifications

In `matching.cc`, you saw how to implement certain C-style functions. In
`compatibility.cc`, you will learn about managing classes and their attributes in C++.
For instance, you may be given a `User` object called `u1` defined as so:
```cpp
User u1;
```
Looking at the `user.hpp` file, you may notice that being a `User` gives you
certain preset (and unset) values. Let's access one of those attributes:
```cpp
std::cout << u1.seriousness << std::endl;
```
This prints out the seriousness of this user. For now, this is 0. We can update
it as so:
```cpp
u1.seriousness = 10;
std::cout << u1.seriousness << std::endl;
```
This code block will now print out `10`. 

Another thing to look at is this code piece that you may see lying around:
```cpp
std::vector<int>
```
We can break this down into parts. `std` refers to the standard library namespace.
In other words, C++ has provided us with generic, basic, built-in data structures
and methods (so long as we include their header files), and labeled them as 
belonging to some `std` library. One example you have already seen is 
`cout` and `endl`, but now we are introducing more. 

`vector` is a data type under the `std` library. You can import it into your
code by having the header
```cpp
#include <vector>
```
at the top of your file. You may be familiar with Python's lists. This is 
essentially a Python list, except with the caveat that all elements within the
vector must be of the same type, and in this case, we've made sure that the 
vector can only hold `int`s.

All this to say, we're creating a dynamic array that can only hold `int` values.

Being a `std::vector` comes with certain methods built in. For instance, the
method `.size()` gives you the length of the vector, and `.push_back()` appends
a value to the end of the vector.
```cpp
std::vector<int> v;
v.push_back(1)
v.push_back(2)
std::cout << v.size() << std::endl; // prints 2
```
The equivalent in Python might look like:
```python
v: list[int] = []
v.append(1)
v.append(2)
print(len(v))
```

You will not have to use very many things from the `std` namespace to complete
this comp assignment, but we will introduce more of them as time continues. 
However, you'll likely have to modify or reference the `user.hpp` file to
complete it, and so our goal is that you will get comfortable with classes 
during this assignment!

### `check_compatibility`

`check_compatibility` is our function that determines if two `User`s are
compatible. Implement the below rules for `User u1` and `User u2` inside
of this function.

Rules:
- Datamatch users can't match with themselves.
- Normally, Datamatch allows for cross-school dates for certain colleges.
For instance, MIT and Harvard students can go on dates if the user prefers.
For this function, assume that this feature has been disabled. If `u1` and `u2`
attend different colleges, they are not compatible.
- Datamatch allows people to seek dates for love, or dates for friendship.
`u1` and `u2` can't be compatible if `u1` is seeking love, and `u2` is seeking 
friendship, and vice versa. `get_match_type()` may be useful here.
- A few years ago, Datamatch implemented a crush roulette system that allows a 
person to "toss a coin into a well" to "increase the chances of being matched."
If `u1` and `u2` are each other's crushes, they skip every rule below (class
year, age, and houses). They still have to pass the rules above: they can't
be the same user, they must attend the same college, and their love/friendship
preferences must fit each other.
- To avoid seniors getting matched with freshman, we make sure that two users
aren't matched if their class year difference is too great. `u1` and `u2` cannot
be farther than `YEAR_DIFF_MAX` in class years to be compatible.
- Some users prefer people around their age (or older, or younger). Enforce the
class year requirements first, then make sure that `u1` and `u2`'s age
preferences (if they exist) align with `u2` and `u1`'s ages, respectively, and
vice versa.
- Some users prefer not to date members within their house. Enforce this rule.
- Some users have blocked matching with anyone from certain houses entirely.
Enforce this rule.

### A new profile factor

As is the norm with Algo team, you will often be tasked with implementing a new
feature for the Datamatch algorithm to take into consideration. In the real
algorithm, that means writing a new **profile factor**.

#### Answer points

Every part of a pair's score is in one unit, **answer points**: roughly one
matching answer on a 12-question survey.

```
score = survey_points + profile_points
```

- `survey_points` (in `scoring.cc`) is how similar two people's survey answers
  are. Matching an answer earns 0.5 to 1.0 points, more when fewer people chose
  it. On real Datamatch data, a typical person's 10 best candidates get about
  **7.7** survey points.
- `profile_points` is the sum of every factor in `factors/registry.cc`. Each
  factor looks at one pair and returns some points: 0 means no effect, and
  negative points are allowed.

These are the factors you're given, with their values from the real algorithm:

| Factor | File | Points |
|---|---|---|
| Mutual crush | `factors/crush.cc` | +16.39 |
| One-way crush | `factors/crush.cc` | +7.06 |
| Same class year / 1 apart / 2 apart | `factors/class_year.cc` | +0.91 / 0 / -1.03 |
| Rice purity | `factors/rice_purity.cc` | -3.43 to +3.12 |
| Both have a bio | `factors/profile.cc` | +0.38 |
| Both answered prompts | `factors/profile.cc` | +0.38 |

Because everything is in one unit, you can compare factors directly: a mutual
crush is worth about twice the whole survey similarity of a typical best
candidate.

#### Your factor

In `factors/my_factor.cc`, implement `my_factor`: given a pair of users
(`pair.u1` and `pair.u2`), return how many answer points your factor gives
them. Then add it to the list in `factors/registry.cc`:

```cpp
{"your_factor_name", my_factor},
```

Keep the function named `my_factor` so the tests can find it; the name in the
registry can be anything.

Some ideas:
- The real algorithm also has a seriousness factor, which isn't included here:
  users whose `seriousness` answers are close get more points.
- Users close in age, or with similar rice purity scores.
- Users in the same house (or deliberately different ones).

Put the point values your factor uses at the top of the file as named
constants (like `MUTUAL_CRUSH_POINTS` in `factors/crush.cc`), with a comment
saying what each one means and **why you chose that many points**. Use the
table above to decide: should your factor matter more or less than having the
same class year? More or less than a one-way crush?

Note:
If you modify the `user.hpp` file, do not modify existing variables.

# Checking your work

You can check whether your code compiles by running:
```bash
make scoring
```
If no error messages appear, the code compiled successfully.

You can check an individual test file by running
```bash
make <test number>
```
Tests 1 to 8 check `check_compatibility`. Test 9 checks that `my_factor` is in
`factors/registry.cc` and gives points to at least some of 200 varied pairs,
and prints the range of points it gave.

You can check what we expect your file to output by running
```bash
make answers-<test number>
```

You can check all of the tests, and some useful output, by running
```bash
check50 ivanharvard/datamatch-algo-comp/main/comp2 --local
```

