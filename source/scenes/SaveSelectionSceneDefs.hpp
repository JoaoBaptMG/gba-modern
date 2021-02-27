//--------------------------------------------------------------------------------
// SaveSelectionSceneDefs.hpp
//--------------------------------------------------------------------------------
// Standard definitions for the save selection scene
//--------------------------------------------------------------------------------
#pragma once

#include "data/backgrounds/save-panel.hpp"

constexpr auto NumSaves = 6;
constexpr auto FirstSBlock = 32 - NumSaves;
constexpr auto ScreenblockSize = sizeof(data::backgrounds::save_panel.png.scrEntries) / NumSaves;
constexpr auto PanelWidth = data::backgrounds::save_panel.png.SeWidth * 8;
constexpr auto PanelHeight = data::backgrounds::save_panel.png.SeHeight / NumSaves * 8;

constexpr auto NumAnimFramesPerPanel = 24;
constexpr auto TotalFrames = NumAnimFramesPerPanel * NumSaves;
