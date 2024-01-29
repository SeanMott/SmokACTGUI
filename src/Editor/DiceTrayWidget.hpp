#pragma once

//defines a GUI for a dice tray

#include <BTDSTD/Logger.hpp>
#include <BTDSTD/String.hpp>

#include <SmokTyGUI/Widgets/Button.hpp>

#include <TableToolkit_Common/Random/Dice.hpp>

#include <imgui.h>

namespace TableToolkit::Editor::Widget
{
	//draws a dice try widget
	static inline void DrawWidget_DiceTray()
	{
		ImGui::Begin("Dice Tray");

		//D6
		if (ImGui::Button("D6", Smok::TyGUI::Util::CalculateLabelSize("D6")))
			BTD::Logger::LogMessage(std::string("D6 Result: " + BTD::Util::ToString(Util::Roll::GenerateD6Result())));

		//D10
		else if (ImGui::Button("D10", Smok::TyGUI::Util::CalculateLabelSize("D10")))
			BTD::Logger::LogMessage(std::string("D10 Result: " + BTD::Util::ToString(Util::Roll::GenerateD10Result())));

		ImGui::End();
	}
}