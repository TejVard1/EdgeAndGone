#include "cricket/server/room.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cricket::server {

namespace {
constexpr auto countdown_duration = std::chrono::seconds(3);
}

Room::Room(std::string code) : code_(normalize_code(code)) {
    if (!is_valid_code(code_)) {
        throw std::invalid_argument("room code must contain eight valid characters");
    }
}

std::string Room::normalize_code(const std::string_view code) {
    std::string normalized;
    normalized.reserve(room_code_length);

    for (const unsigned char character : code) {
        if (character == '-' || std::isspace(character) != 0) {
            continue;
        }
        normalized.push_back(static_cast<char>(std::toupper(character)));
    }

    return normalized;
}

bool Room::is_valid_code(const std::string_view code) noexcept {
    return code.size() == room_code_length &&
           std::all_of(code.begin(), code.end(), [](const char character) {
               return room_code_alphabet.find(character) != std::string_view::npos;
           });
}

const std::string& Room::code() const noexcept {
    return code_;
}

RoomPhase Room::phase() const noexcept {
    return phase_;
}

const std::optional<PlayerSeat>& Room::player(const std::size_t index) const {
    if (index >= players_.size()) {
        throw std::out_of_range("player index is outside the room");
    }
    return players_[index];
}

bool Room::is_full() const noexcept {
    return players_[0].has_value() && players_[1].has_value();
}

std::size_t Room::join(PlayerSeat player) {
    if (phase_ == RoomPhase::in_match || phase_ == RoomPhase::closed) {
        throw std::logic_error("room is not accepting players");
    }
    if (player.display_name.empty() || player.session_id.empty()) {
        throw std::invalid_argument("player name and session ID are required");
    }

    for (std::size_t index = 0; index < players_.size(); ++index) {
        if (!players_[index].has_value()) {
            player.ready = false;
            players_[index] = std::move(player);
            update_lobby_phase(std::chrono::steady_clock::now());
            return index;
        }
    }

    throw std::logic_error("room is full");
}

void Room::set_ready(const std::size_t player_index, const bool ready) {
    if (player_index >= players_.size() || !players_[player_index].has_value()) {
        throw std::out_of_range("player seat does not exist");
    }
    if (phase_ == RoomPhase::in_match || phase_ == RoomPhase::closed) {
        throw std::logic_error("ready state cannot change now");
    }

    players_[player_index]->ready = ready;
    update_lobby_phase(std::chrono::steady_clock::now());
}

void Room::set_connected(const std::size_t player_index, const bool connected) {
    if (player_index >= players_.size() || !players_[player_index].has_value()) {
        throw std::out_of_range("player seat does not exist");
    }

    players_[player_index]->connected = connected;
    if (!connected) {
        players_[player_index]->ready = false;
    }
    update_lobby_phase(std::chrono::steady_clock::now());
}

void Room::refresh(const std::chrono::steady_clock::time_point now) {
    if (phase_ == RoomPhase::countdown && countdown_deadline_.has_value() &&
        now >= *countdown_deadline_) {
        start_match(now);
    }
}

void Room::start_match(const std::chrono::steady_clock::time_point now) {
    if (phase_ != RoomPhase::countdown || !countdown_deadline_.has_value() ||
        now < *countdown_deadline_) {
        throw std::logic_error("countdown has not completed");
    }

    phase_ = RoomPhase::in_match;
    countdown_deadline_.reset();
}

std::optional<std::chrono::steady_clock::time_point> Room::countdown_deadline() const noexcept {
    return countdown_deadline_;
}

void Room::update_lobby_phase(const std::chrono::steady_clock::time_point now) {
    if (phase_ == RoomPhase::in_match || phase_ == RoomPhase::closed) {
        return;
    }

    if (!is_full()) {
        phase_ = RoomPhase::waiting_for_opponent;
        countdown_deadline_.reset();
        return;
    }

    const bool both_connected = players_[0]->connected && players_[1]->connected;
    const bool both_ready = players_[0]->ready && players_[1]->ready;

    if (both_connected && both_ready) {
        if (phase_ != RoomPhase::countdown) {
            phase_ = RoomPhase::countdown;
            countdown_deadline_ = now + countdown_duration;
        }
        return;
    }

    phase_ = RoomPhase::ready_check;
    countdown_deadline_.reset();
}

}  // namespace cricket::server