#include <fstream>

#include "config.h"
#include "inputs.h"

static inline Gender str2gender(const std::string s) {
    if (s == "man") {
        return MALE;
    } else if (s == "woman") {
        return FEMALE;
    } else if (s == "nonbinary") {
        return NONBINARY;
    }
    return UNSPECIFIED;
}

static std::vector<int> getrandanswers(size_t num_questions, size_t num_options) {
    std::vector<int> randomized;
    while (num_questions--) {
        randomized.push_back(rand() % num_options);
    }
    return randomized;
}

static void create_user_from_json(
    const std::string uid, const json& private_info, const json& public_info,
    const std::string curr_college, const json& responses,
    const size_t nquestions, std::vector<User>& users,
    std::vector<std::string>& uids, std::vector<std::string>& emails, size_t num_options
) {
    // logger.log(INFO, "curr college: %s", curr_college.c_str());

    // printf("public info uid: %s\n", public_info[uid].dump().c_str());
    // printf("curr_college: %s\n", curr_college.c_str());
    // printf("public info uid college: %s\n\n", public_info[uid]["college"].dump().c_str());

    if (!private_info.contains(uid) ||
        !public_info.contains(uid) ||
        !public_info[uid].contains("college") ||
        curr_college.find(public_info[uid]["college"]) == std::string::npos) {
        // printf("User not found or not assigned to college\n");
        return;
    }


    std::string college = public_info[uid]["college"];

    if ((college == "MIT" || college == "Harvard") && curr_college == "Harvard-MIT") {
        return;
    }
    // logger.log(INFO, "user college: %s", college.c_str());

    assert(college == curr_college);

    User u;
    u.id = uid;
    u.college = college;
    u.email = private_info[uid]["email"];

    try {
        u.answers = responses[uid].get<std::vector<int>>();
    } catch (const std::exception& e) {
        u.answers = getrandanswers(nquestions, num_options);
    }

    if (u.answers.size() < nquestions) {
        u.answers = getrandanswers(nquestions, num_options);
    }

    if (private_info[uid].contains("gender") &&
        private_info[uid]["gender"].contains("genderValue")) {
        u.gender = str2gender(private_info[uid]["gender"]["genderValue"]);
    } else {
        u.gender = UNSPECIFIED;
    }

    if (private_info[uid].contains("matchCategory")) {
        u.seriousness = private_info[uid]["matchCategory"];
    }

    u.has_prompts = private_info[uid].contains("prompt");

    if (private_info[uid].contains("description")) {
        u.bio = private_info[uid]["description"];
    }

    /* START Dealbreakers */
    if (private_info[uid].contains("age")) {
        u.age = private_info[uid]["age"];
    } else if (public_info[uid].contains("age")) {
        u.age = public_info[uid]["age"];
    }

    if (private_info[uid].contains("religion")) {
        u.religion = private_info[uid]["religion"].get<std::string>();
    } else if (public_info[uid].contains("religion")) {
        u.religion = public_info[uid]["religion"].get<std::string>();
    }

    if (private_info[uid].contains("dealbreakers")) {
        if (private_info[uid]["dealbreakers"].contains("restrictAge") && private_info[uid]["dealbreakers"]["restrictAge"].get<bool>() && private_info[uid]["dealbreakers"].contains("ageRange")) {
            int age_gap = private_info[uid]["dealbreakers"]["ageRange"].get<int>();
            u.ageRange = age_gap;
        }

        if (private_info[uid]["dealbreakers"].contains("dorms") && private_info[uid]["dealbreakers"]["dorms"].is_array()) {
            for (const auto& dorm: private_info[uid]["dealbreakers"]["dorms"]) {
                u.blocked_houses.push_back(dorm.get<std::string>());
            }
        }
        if (private_info[uid]["dealbreakers"].contains("restrictReligion") && private_info[uid]["dealbreakers"]["restrictReligion"].get<bool>()) {
            u.restrict_religion = true;
        }
    }
    /* END Dealbreakers */

    if (public_info[uid].contains("year")) {
        std::string year = public_info[uid]["year"];
        // logger.log(INFO, "year: %s", year.c_str());
        if (year == "grad" || year == "alumni") {
            u.year = 5;
        } else {
            u.year = FRESHMAN_CLASS - std::stoi(year);
        }
    }

    if (public_info[uid].contains("dorm")) {
        u.house = public_info[uid]["dorm"];
    }

    // TODO: Read "noDormMatch" from the user's private profile into
    // u.no_house_matches. Some users don't have it (see the README).

    // Gender preferences for love and friendship
    if (private_info[uid].contains("lookingForGender")) {
        std::string prefs;
        if (private_info[uid]["lookingForGender"].contains("love")) {
            prefs = private_info[uid]["lookingForGender"]["love"];
            u.loveprefs = get_gender_prefs(prefs);
        }
        if (private_info[uid]["lookingForGender"].contains("friendship")) {
            prefs = private_info[uid]["lookingForGender"]["friendship"];
            u.friendprefs = get_gender_prefs(prefs);
        }
    } else {
        // If nothing is specified, then default to preferring everybody for
        // friendship
        u.friendprefs = get_gender_prefs("people of all genders");
    }

    // mbti
    if (public_info[uid].contains("mbti")) {
        u.mbti = public_info[uid]["mbti"];
    }

    // rice purity
    if (public_info[uid].contains("ricePurityScore")) {
        u.ricepurity = public_info[uid]["ricePurityScore"];
    }

    // costar
    if (public_info[uid].contains("zodiac")) {
        if (public_info[uid]["zodiac"].contains("sign")) {
            u.costar = public_info[uid]["zodiac"]["sign"];
        }
    }

    // Add newly-created user
    users.push_back(u);
    uids.push_back(uid);
    emails.push_back(private_info[uid]["email"]);
}

bool load_college_names(const std::string& answer_choice_sims_path,
                        std::vector<std::string>& colleges) {
    Logger& logger = Logger::instance();
    printf("Checking if college_metadata.json exists in the answer_choice_sims path...\n");

    std::string college_metadata_file = answer_choice_sims_path + std::string("/college_metadata.json");
    std::ifstream college_ifs(college_metadata_file.c_str());
    if (!college_ifs) {
        logger.log(ERROR, "File %s not found", college_metadata_file.c_str());
        return false;
    }
    json college_md;
    college_ifs >> college_md;

    // college_md => {"Harvard": {...}, "MIT": {...}, ...}
    colleges.reserve(college_md.size());
    for (const auto& [key, _] : college_md.items()) {
        colleges.push_back(key);
    }
    return true;
}

bool load_answer_choice_sims(const std::string& answer_choice_sims_path,
                             SchoolInput& school, College& college) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Reading in cosine similarities between answer choices");

    // Read in cosine sims for answer choices
    json cosine_similarities;
    // Need to make sure output file from SB cos sim is separated by college
    std::string filename =
        answer_choice_sims_path + "/answer_choice_sims_" + school.name + ".json";
    std::ifstream cosinein(filename);
    if (!cosinein) {
        logger.log(ERROR, "File %s not found", filename.c_str());
        return false;
    }
    cosinein >> cosine_similarities;
    json cosine_sims_list = cosine_similarities["data"];
    const size_t nquestions = cosine_similarities["num_questions"];
    const size_t num_options = cosine_sims_list[0].at("sim_matrix")[0].size();

    Matrix3D<float> cosine_sims(nquestions, num_options, num_options);
    for (size_t q = 0; q < nquestions; ++q) {
        json cosine_sims_matrix = cosine_sims_list[q].at("sim_matrix");

        // make sure dimensions are correct
        if (cosine_sims_matrix.size() != num_options ||
            cosine_sims_matrix[0].size() != num_options) {
            logger.log(
                ERROR,
                "Cosine similarity matrix shape mismatch at q=%zu", q
            );
            return false;
        }

        for (size_t i = 0; i < num_options; ++i) {
            for (size_t j = 0; j < num_options; ++j) {
                cosine_sims(q, i, j) = cosine_sims_matrix[i][j].get<float>();
            }
        }
    }

    school.nquestions = nquestions;
    school.num_options = num_options;
    college.name = school.name;
    college.cosine_sims = std::move(cosine_sims);
    // autofilled with zeros; filled in by compute_answer_distribution()
    college.answer_dist = Matrix2D<float>(nquestions, num_options);
    return true;
}

void load_users(const json& full, SchoolInput& school) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Reading in user profiles and answers");

    const json& private_info = full.at("privateProfile");
    const json& public_info = full.at("publicProfile");
    const json& answers = full.at("responses");

    for (auto it = answers.begin(); it != answers.end(); ++it) {
        const std::string& uid = it.key();
        try {
            create_user_from_json(uid, private_info, public_info,
            school.name, answers, school.nquestions, school.users,
            school.uids, school.emails, 5);
            // TODO: may need to change this to 4 during testing
        } catch (const nlohmann::json::type_error& e) {
            fprintf(stderr, "JSON type_error for uid=%s: %s\n", uid.c_str(), e.what());

            // Dump likely offenders
            if (public_info.contains(uid)) {
                std::string public_str = public_info[uid].dump();
                fprintf(stderr, "public_info[uid]=%s\n", public_str.c_str());
            }
            if (private_info.contains(uid)) {
                std::string private_str = private_info[uid].dump();
                fprintf(stderr, "private_info[uid]=%s\n", private_str.c_str());
            }
            throw;
        }
    }

    logger.log(INFO, "Found %zu users\n", school.users.size());

    assert(school.users.size() == school.uids.size() &&
           school.uids.size() == school.emails.size());
}

void compute_answer_distribution(const SchoolInput& school, College& college) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Creating answer distributions");

    const size_t nquestions = school.nquestions;
    const size_t num_options = school.num_options;
    auto& answer_dist = college.answer_dist;

    for (const User& u : school.users) {
        for (size_t i = 0; i < u.answers.size(); ++i) {
            int a = u.answers[i];
            if (i < nquestions && a >= 0 && static_cast<size_t>(a) < num_options) {
                answer_dist(i, a) += 1.0;
            }
        }
    }

    logger.log(INFO, "Calculating answer distributions and expected similarity");

    for (size_t i = 0; i < nquestions; ++i) {
        for (size_t j = 0; j < num_options; ++j) {
            // normalize and calculate expected similarity
            answer_dist(i, j) /= static_cast<float>(school.users.size());
            college.dist_min = std::min(college.dist_min, answer_dist(i, j));
            college.exp_sim += answer_dist(i, j) * answer_dist(i, j);
        }
    }
}

bool load_pair_matrix(const std::string& filename, size_t tucount,
                      Matrix2D<float>& out) {
    Logger& logger = Logger::instance();
    std::ifstream in(filename);
    if (!in) {
        logger.log(ERROR, "File %s not found", filename.c_str());
        return false;
    }

    json similarities;
    in >> similarities;
    json data = similarities.at("data");

    out = Matrix2D<float>(tucount, tucount);
    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = 0; j < tucount; ++j) {
            out(i, j) = data[i][j];
        }
    }
    return true;
}
