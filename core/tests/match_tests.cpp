#include "cricket/match.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {

void require(const bool condition, const std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void test_toss_is_deterministic() {
    cricket::Match first;
    cricket::Match second;

    first.conduct_toss(20260906);
    second.conduct_toss(20260906);

    require(first.toss_winner() == second.toss_winner(), "same seed must produce same toss");
    require(first.phase() == cricket::MatchPhase::awaiting_toss_decision,
            "toss must wait for winner decision");
}

void test_wides_do_not_consume_legal_balls() {
    cricket::Match match;
    match.conduct_toss(42);
    match.choose_after_toss(cricket::TossDecision::bat);

    match.record_delivery({0, 1, cricket::ExtraType::wide, false});

    require(match.innings().runs == 1, "wide must add its extra run");
    require(match.innings().legal_balls == 0, "wide must not consume a legal ball");
}

void test_five_over_innings_and_chase() {
    cricket::Match match;
    match.conduct_toss(7);
    match.choose_after_toss(cricket::TossDecision::bat);
    const int first_batting_player = match.batting_player();

    for (int ball = 0; ball < 30; ++ball) {
        match.record_delivery({ball < 10 ? 1 : 0, 0, cricket::ExtraType::none, false});
    }

    require(match.phase() == cricket::MatchPhase::innings_break,
            "thirty legal balls must end a five-over innings");
    require(match.first_innings_total() == 10, "first innings score must be retained");
    require(match.target() == 11, "target must be one more than first innings score");

    match.start_second_innings();
    require(match.batting_player() != first_batting_player, "batting side must swap");

    match.record_delivery({6, 0, cricket::ExtraType::none, false});
    match.record_delivery({6, 0, cricket::ExtraType::none, false});

    require(match.phase() == cricket::MatchPhase::complete, "successful chase must end match");
    require(match.innings().legal_balls == 2, "chase should finish on the winning delivery");
    require(match.winner() == match.batting_player(), "chasing player must win");
}

void test_score_format() {
    cricket::Match match;
    match.conduct_toss(100);
    match.choose_after_toss(cricket::TossDecision::bowl);

    for (int ball = 0; ball < 7; ++ball) {
        match.record_delivery({1, 0, cricket::ExtraType::none, ball == 3});
    }

    require(match.score_text() == "7/1 (1.1)", "score must use cricket over notation");
}

}  // namespace

int main() {
    try {
        test_toss_is_deterministic();
        test_wides_do_not_consume_legal_balls();
        test_five_over_innings_and_chase();
        test_score_format();
        std::cout << "All cricket core tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}