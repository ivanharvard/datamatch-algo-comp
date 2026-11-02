#ifndef OUTPUT_H
#define OUTPUT_H

// Stage 5: writing a school's matches to data/results/.

#include <string>

#include "inputs.h"
#include "match_rounds.h"
#include "score_matrix.h"

// Folder that results are written to, relative to where ./main is run.
extern const std::string RESULTS_DIR;

// Creates data/ and data/results/ if they don't exist yet.
void make_results_dir();

// Writes matches_<school>.json and matchCatalog_<school>.json.
void write_results(const SchoolInput& school, const ScoreMatrices& scored,
                   const MatchResults& results);

#endif
