#include <cmath>

#include "crush_roulette.h"

void load_crushes(const json& full, SchoolInput& school, CrushRoulette& crush) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Reading in crush roulette data");

    const json& private_info = full.at("privateProfile");
    const json& crushes = full.at("crushes");
    const size_t tucount = school.users.size();
    const std::vector<std::string>& uids = school.uids;
    const std::vector<std::string>& emails = school.emails;

    // A crush roulette pairing including myself is a ship unless the person
    // submitted themselves
    crush.shipmat = Matrix2D<uint16_t>(tucount, tucount);
    crush.crushmat = Matrix2D<uint8_t>(tucount, tucount);

    for (auto it = crushes.begin(); it != crushes.end(); ++it) {
        std::string uid = it.key();
        size_t uid_idx = get_index(uids, uid);
        if (uid_idx < tucount && private_info.contains(uid)) {
            // TODO: Check that this actually fetches the right uid
            // because I think it should be searching emails
            size_t email1_idx = get_index(emails, crushes[uid]["email1"]);
            size_t email2_idx = get_index(emails, crushes[uid]["email2"]);
            if (email1_idx < tucount && email2_idx < tucount) {
                if (email1_idx == uid_idx) {
                    school.users[uid_idx].crush = uids[email2_idx];
                    crush.crushmat(uid_idx, email2_idx) = 1;
                } else if (email2_idx == uid_idx) {
                    school.users[uid_idx].crush = uids[email1_idx];
                    crush.crushmat(uid_idx, email1_idx) = 1;
                } else {
                    ++crush.shipmat(email1_idx, email2_idx);
                    ++crush.shipmat(email2_idx, email1_idx);
                }
            }
        }
    }
}

void compute_ship_stats(size_t tucount, CrushRoulette& crush) {
    const Matrix2D<uint16_t>& shipmat = crush.shipmat;
    float num_ships = 0;
    float ship_mu = 0;
    float ship_sig = 0;

    for (size_t i = 0; i < tucount - 1; ++i) {
        for (size_t j = i + 1; j < tucount; ++j) {
            if (shipmat(i, j) > 0) {
                ++num_ships;
                ship_mu += shipmat(i, j);
            }
        }
    }

    if (num_ships > 0) {
        ship_mu /= num_ships;
    }

    for (size_t i = 0; i < tucount - 1; ++i) {
        for (size_t j = i + 1; j < tucount; ++j) {
            if (shipmat(i, j) > 0) {
                ship_sig += (shipmat(i, j) - ship_mu)
                          * (shipmat(i, j) - ship_mu);
            }
        }
    }

    if (num_ships > 0) {
        ship_sig = sqrt(ship_sig / num_ships);
    }

    crush.ship_mu = ship_mu;
    crush.ship_sig = ship_sig;
}
