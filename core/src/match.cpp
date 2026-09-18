#include "cricket/match.hpp"

#include <sstream>
#include <stdexcept>

namespace cricket {

bool DeliveryOutcome::is_legal() const noexcept {
    return extra_type != ExtraType::wide && extra_type != ExtraType::no_ball;
}

int DeliveryOutcome::total_runs() const noexcept {
    return runs_off_bat + extra_runs;
}

int InningsState::completed_overs() const noexcept {
    return legal_balls / balls_per_over;
}

int InningsState::balls_in_current_over() const noexcept {
    return legal_balls % balls_per_over;
}

Match::Match(MatchConfig config) : config_(config) {
    if (config_.overs_per_innings <= 0) {
        throw std::invalid_argument("overs_per_innings must be positive");
    }
    if (config_.wickets_per_innings <= 0) {
        throw std::invalid_argument("wickets_per_innings must be positive");
    }
}

void Match::conduct_toss(const std::uint64_t seed) {
    if (phase_ != MatchPhase::awaiting_toss) {
        throw std::logic_error("toss can only be conducted once");
    }

    // SplitMix64 finalizer gives stable, well-distributed bits for a recorded seed.
    std::uint64_t value = seed + 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    value ^= value >> 31U;

    toss_winner_ = static_cast<int>(value & 1ULL);
    phase_ = MatchPhase::awaiting_toss_decision;
}

void Match::choose_after_toss(const TossDecision decision) {
    if (phase_ != MatchPhase::awaiting_toss_decision) {
        throw std::logic_error("the toss winner cannot choose in the current phase");
    }

    batting_player_ = decision == TossDecision::bat ? toss_winner_ : 1 - toss_winner_;
    phase_ = MatchPhase::first_innings;
}

void Match::record_delivery(const DeliveryOutcome& outcome) {
    if (phase_ != MatchPhase::first_innings && phase_ != MatchPhase::second_innings) {
        throw std::logic_error("a delivery requires an active innings");
    }
    if (outcome.runs_off_bat < 0 || outcome.extra_runs < 0) {
        throw std::invalid_argument("runs cannot be negative");
    }
    if (outcome.extra_type == ExtraType::none && outcome.extra_runs != 0) {
        throw std::invalid_argument("extra runs require an extra type");
    }
    if (outcome.extra_type != ExtraType::none && outcome.extra_runs == 0) {
        throw std::invalid_argument("an extra type requires extra runs");
    }

    innings_.runs += outcome.total_runs();
    if (outcome.wicket) {
        ++innings_.wickets;
    }
    if (outcome.is_legal()) {
        ++innings_.legal_balls;
    }

    if (phase_ == MatchPhase::second_innings && innings_.runs >= target()) {
        winner_ = batting_player_;
        phase_ = MatchPhase::complete;
        return;
    }

    if (innings_is_complete()) {
        finish_current_innings();
    }
}

void Match::start_second_innings() {
    if (phase_ != MatchPhase::innings_break) {
        throw std::logic_error("second innings can only start after the innings break");
    }

    batting_player_ = 1 - batting_player_;
    innings_ = {};
    phase_ = MatchPhase::second_innings;
}

MatchPhase Match::phase() const noexcept {
    return phase_;
}

int Match::toss_winner() const noexcept {
    return toss_winner_;
}

int Match::batting_player() const noexcept {
    return batting_player_;
}

int Match::bowling_player() const noexcept {
    return batting_player_ < 0 ? -1 : 1 - batting_player_;
}

const InningsState& Match::innings() const noexcept {
    return innings_;
}

int Match::first_innings_total() const noexcept {
    return first_innings_total_;
}

int Match::target() const noexcept {
    return first_innings_total_ < 0 ? -1 : first_innings_total_ + 1;
}

int Match::winner() const noexcept {
    return winner_;
}

std::string Match::score_text() const {
    std::ostringstream output;
    output << innings_.runs << '/' << innings_.wickets << " ("
           << innings_.completed_overs() << '.'
           << innings_.balls_in_current_over() << ')';
    return output.str();
}

bool Match::innings_is_complete() const noexcept {
    return innings_.wickets >= config_.wickets_per_innings ||
           innings_.legal_balls >= config_.overs_per_innings * balls_per_over;
}

void Match::finish_current_innings() {
    if (phase_ == MatchPhase::first_innings) {
        first_innings_total_ = innings_.runs;
        phase_ = MatchPhase::innings_break;
        return;
    }

    if (innings_.runs == first_innings_total_) {
        winner_ = -1;
    } else {
        winner_ = innings_.runs > first_innings_total_ ? batting_player_ : 1 - batting_player_;
    }
    phase_ = MatchPhase::complete;
}

}  // namespace cricket