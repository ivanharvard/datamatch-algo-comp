#include "checking.h"
#include "user.h"
#include "utils.h"
#include "types.h"
#include "lib/json.hpp"

template <typename T>
static inline bool overlaps(const std::set<T> list1, const T val1,
                            const std::set<T> list2, const T val2) {
    return list1.find(val2) != list1.end() || list2.find(val1) != list2.end();
}

template <typename T>
static inline bool overlaps_bidirect(const std::set<T> list1, const T val1,
                                     const std::set<T> list2, const T val2) {
    return list1.find(val2) != list1.end() && list2.find(val1) != list2.end();
}

void check_blocklist(std::vector<User>& users,
                     std::vector<std::pair<size_t, size_t>>& matches) {
    Logger logger = Logger::instance();
    logger.log(INFO, "Checking blocklist...");
    for (auto match : matches) {
        size_t u1 = match.first;
        size_t u2 = match.second;
        assert(u1 < u2);
        User* user1 = &users[u1];
        User* user2 = &users[u2];
        bool error = overlaps(user1->blocklist, user1->email, user2->blocklist,
                              user2->email);
        if (error) {
            logger.log(ERROR, "Blocklist error: User %s (%s) and %s (%s) "
                "matched despite being blocked", user1->email.c_str(),
                user1->id.c_str(), user2->email.c_str(), user2->id.c_str());
        }
    }
}

void check_gender_prefs(std::vector<User>& users,
                        std::vector<std::pair<size_t, size_t>>& matches,
                        Matrix2D<int8_t>& matchtypes) {
    Logger logger = Logger::instance();
    logger.log(INFO, "Checking gender prefs for %zu matches", matches.size());
    for (auto match : matches) {
        size_t u1 = match.first;
        size_t u2 = match.second;
        assert(u1 < u2);
        User* user1 = &users[u1];
        User* user2 = &users[u2];
        int matchtype = matchtypes(u1, u2);
        bool error = false;
        if (matchtype == 1) {
            error = !overlaps_bidirect(user1->loveprefs, user1->gender,
                                       user2->loveprefs, user2->gender);
        } else if (matchtype == 0) {
            error = !overlaps_bidirect(user1->friendprefs, user1->gender,
                                       user2->friendprefs, user2->gender);
        } else {
            error = true;
        }

        if (error) {
            logger.log(ERROR, "Gender prefs error: User %s (%s) and %s (%s) "
                "have invalid gender prefs for matchtype %d",
                user1->email.c_str(), user1->id.c_str(), user2->email.c_str(),
                user2->id.c_str(), matchtype);
        }
    }
}
