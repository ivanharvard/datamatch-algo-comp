#ifndef INPUTS_H
#define INPUTS_H

// Stage 1: reading input files and building the list of users for a school.

#include <string>
#include <vector>

#include "scoring.h"
#include "types.h"
#include "user.h"
#include "utils.h"

// Everything read from the input files for one school.
struct SchoolInput {
    std::string name;
    size_t nquestions = 0;
    size_t num_options = 0;
    std::vector<User> users;
    std::vector<std::string> uids;    // uids[i] == users[i].id
    std::vector<std::string> emails;  // emails[i] is users[i]'s email
};

// Reads the list of school names from <answer_choice_sims_path>/college_metadata.json.
// Returns false (after logging) if the file is missing.
bool load_college_names(const std::string& answer_choice_sims_path,
                        std::vector<std::string>& colleges);

// Reads the answer-choice similarity file for one school into `college` and
// sets the number of questions and options per question.
// Returns false (after logging) if the file is missing or a question's
// similarity matrix isn't num_options x num_options.
bool load_answer_choice_sims(const std::string& answer_choice_sims_path,
                             SchoolInput& school, College& college);

// Builds school.users (plus uids and emails) from the Firebase export.
void load_users(const json& full, SchoolInput& school);

// Fills in college.answer_dist, college.dist_min and college.exp_sim from
// the users' survey answers.
void compute_answer_distribution(const SchoolInput& school, College& college);

// Reads an N x N similarity file ({"data": [[...]]}) into `out`.
// Returns false (after logging) if the file is missing, and throws a JSON
// type_error if it has fewer than tucount rows or columns.
//
// Assumes row/column i is the same person as school.users[i]. Nothing in the
// file records which user a row belongs to, so this is not checked; the
// Python scripts that write these files currently order users differently
// (all profiles in publicProfile order, vs. survey respondents in sorted
// uid order here), so rows can be misattributed.
bool load_pair_matrix(const std::string& filename, size_t tucount,
                      Matrix2D<float>& out);

#endif
