#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace cricket {

inline constexpr std::string_view core_version = "0.1.0";
inline constexpr int balls_per_over = 6;

enum class MatchPhase {
    awaiting_toss,
    awaiting_toss_decision,
    first_innings,
    innings_break,
    second_innings,
    complete
};

enum class TossDecision {
    bat,
    bowl
};

enum class ExtraType {
    none,
    wide,
    no_ball,
    bye,
    leg_bye
};

struct DeliveryOutcome {
    int runs_off_bat{0};
    int extra_runs{0};
    ExtraType extra_type{ExtraType::none};
    bool wicket{false};

    [[nodiscard]] bool is_legal() const noexcept;
    [[nodiscard]] int total_runs() const noexcept;
};

struct InningsState {
    int runs{0};
    int wickets{0};
    int legal_balls{0};

    [[nodiscard]] int completed_overs() const noexcept;
    [[nodiscard]] int balls_in_current_over() const noexcept;
};

struct MatchConfig {
    int overs_per_innings{5};
    int wickets_per_innings{10};
};

class Match {
public:
    explicit Match(MatchConfig config = {});

    void conduct_toss(std::uint64_t seed);
    void choose_after_toss(TossDecision decision);
    void record_delivery(const DeliveryOutcome& outcome);
    void start_second_innings();

    [[nodiscard]] MatchPhase phase() const noexcept;
    [[nodiscard]] int toss_winner() const noexcept;
    [[nodiscard]] int batting_player() const noexcept;
    [[nodiscard]] int bowling_player() const noexcept;
    [[nodiscard]] const InningsState& innings() const noexcept;
    [[nodiscard]] int first_innings_total() const noexcept;
    [[nodiscard]] int target() const noexcept;
    [[nodiscard]] int winner() const noexcept;
    [[nodiscard]] std::string score_text() const;

private:
    [[nodiscard]] bool innings_is_complete() const noexcept;
    void finish_current_innings();

    MatchConfig config_;
    MatchPhase phase_{MatchPhase::awaiting_toss};
    InningsState innings_{};
    int toss_winner_{-1};
    int batting_player_{-1};
    int first_innings_total_{-1};
    int winner_{-1};
};

}  // namespace cricket