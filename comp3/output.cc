#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>

#include <sys/stat.h>

#include "config.h"
#include "output.h"

const std::string RESULTS_DIR = "data/results";

void make_results_dir() {
    struct stat st;
    if (stat("data", &st) == -1) {
        mkdir("data", (mode_t)0777);
    }
    if (stat(RESULTS_DIR.c_str(), &st) == -1) {
        mkdir(RESULTS_DIR.c_str(), (mode_t)0777);
    }
}

void write_results(const SchoolInput& school, const ScoreMatrices& scored,
                   const MatchResults& results) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Begin writing out matches");

    const std::vector<User>& users = school.users;
    const size_t nquestions = school.nquestions;
    const Matrix2D<int8_t>& matchtypes = scored.matchtypes;
    const Matrix2D<long long>& boostedscores = results.boostedscores;
    const std::vector<long long>& matchscores = results.matchscores;

    std::map<std::string, std::map<std::string, json>> mpairs;
    std::map<std::string, std::map<std::string, json>> mcatalog;

    for (auto p : results.matches) {
        size_t u1 = p.first;
        size_t u2 = p.second;
        assert(u1 < u2);

        std::string couple = users[u1].id + "-" + users[u2].id;
        if (mpairs.find(couple) != mpairs.end()) {
            continue;
        }

        std::vector<long long>::const_iterator low;
        low = std::lower_bound(matchscores.begin(), matchscores.end(),
                            boostedscores(u1, u2));
        size_t ind = low - matchscores.begin();

        std::map<std::string, json> match;
        match["rating"] = (double)ind / matchscores.size() * 0.15 + 0.85;
        match["canDate"] = boostedscores(u1, u2) > SPONSORED_CUTOFF;
        match["relationshipType"] =
            matchtypes(u1, u2) ? "true love" : "friendship";

        // Report matching survey responses alongside each match
        if (nquestions > 0) {
            json matching_answers = json::array();
            for (size_t q = 0; q < nquestions; ++q) {
                if (q < users[u1].answers.size() && q < users[u2].answers.size() &&
                    users[u1].answers[q] == users[u2].answers[q]) {
                    int response = users[u1].answers[q];
                    matching_answers.push_back({{"question", q}, {"response", response}});
                }
            }
            match["matchingResponses"] = matching_answers;
        }
        mpairs[couple] = match;

        // Add to match mcatalog bidirectionally
        mcatalog[users[u1].id][users[u2].id]["matched"] = false;
        mcatalog[users[u2].id][users[u1].id]["matched"] = false;
    }

    // Checkpoint results for school
    std::string mpfilename = RESULTS_DIR + "/matches_" + school.name + ".json";
    {
        std::ofstream mpairs_file(mpfilename);
        if (mpairs_file.is_open()) {
            json mpairs_json(mpairs);
            mpairs_file << std::setw(2) << mpairs_json << "\n";
        }
    }

    std::string mcfilename =
        RESULTS_DIR + "/matchCatalog_" + school.name + ".json";
    {
        std::ofstream mcatalog_file(mcfilename);
        if (mcatalog_file.is_open()) {
            json mcatalog_json(mcatalog);
            mcatalog_file << std::setw(2) << mcatalog_json << "\n";
        }
    }

    printf("Wrote %s results out to %s and %s!\n", school.name.c_str(),
           mpfilename.c_str(), mcfilename.c_str());
}
