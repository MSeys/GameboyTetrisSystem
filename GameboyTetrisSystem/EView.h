#pragma once
#include "pch.h"
#include <utility>

// Class originates from my personal (private) project - https://matthiasseys.me/Projects/crea2dengine.html
class EView
{
public:
	EView(std::string name = "EView - Default", const ImGuiWindowFlags& flags = 0) : m_Name(std::move(name)), m_Flags(flags) { }
	virtual ~EView() = default;

	virtual void Update() = 0; // Added for GW
	virtual void DrawGUI() = 0;

	bool IsOpen() const { return m_IsOpen; }

	void SetSystem(TetrisSystem* pSystem) { m_pSystem = pSystem; } // Added for GW
	void SetName(const std::string& name) { m_Name = name; }
	void SetFlags(const ImGuiWindowFlags flags) { m_Flags = flags; }
	void Close() { m_IsOpen = false; }
	
protected:
	std::string m_Name{};
	bool m_IsOpen{ true };
	ImGuiWindowFlags m_Flags{};
	TetrisSystem* m_pSystem;
};