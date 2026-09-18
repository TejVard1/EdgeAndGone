#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace cricket::server {

inline constexpr std::size_t room_code_length = 8;
inline constexpr std::string_view room_code_alphabet =
    "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

enum class RoomPhase {
    waiting_for_opponent,
    ready_check,
    countdown,
    in_match,
    closed
};

struct PlayerSeat {
    std::string display_name;
    std::string session_id;
    bool connected{true};
    bool ready{false};
};

class Room {
public:
    explicit Room(std::string code);

    [[nodiscard]] static std::string normalize_code(std::string_view code);
    [[nodiscard]] static bool is_valid_code(std::string_view code) noexcept;

    [[nodiscard]] const std::string& code() const noexcept;
    [[nodiscard]] RoomPhase phase() const noexcept;
    [[nodiscard]] const std::optional<PlayerSeat>& player(std::size_t index) const;
    [[nodiscard]] bool is_full() const noexcept;

    std::size_t join(PlayerSeat player);
    void set_ready(std::size_t player_index, bool ready);
    void set_connected(std::size_t player_index, bool connected);
    void refresh(std::chrono::steady_clock::time_point now);
    void start_match(std::chrono::steady_clock::time_point now);

    [[nodiscard]] std::optional<std::chrono::steady_clock::time_point>
    countdown_deadline() const noexcept;

private:
    void update_lobby_phase(std::chrono::steady_clock::time_point now);

    std::string code_;
    RoomPhase phase_{RoomPhase::waiting_for_opponent};
    std::array<std::optional<PlayerSeat>, 2> players_{};
    std::optional<std::chrono::steady_clock::time_point> countdown_deadline_;
};

}  // namespace cricket::server