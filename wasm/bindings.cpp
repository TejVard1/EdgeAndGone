#include "cricket/match.hpp"

#include <emscripten/bind.h>

#include <string>

namespace
{

std::string core_version_string()
{
    return std::string(cricket::core_version);
}

std::string preview_score(const int runs, const int wickets, const int legal_balls)
{
    cricket::InningsState innings{runs, wickets, legal_balls};
    return std::to_string(innings.runs) + "/" + std::to_string(innings.wickets) + " (" +
           std::to_string(innings.completed_overs()) + "." +
           std::to_string(innings.balls_in_current_over()) + ")";
}

} // namespace

EMSCRIPTEN_BINDINGS(boundary_cricket_core)
{
    emscripten::function("coreVersion", &core_version_string);
    emscripten::function("previewScore", &preview_score);
}
