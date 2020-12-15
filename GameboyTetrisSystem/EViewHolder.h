#pragma once
#include <list>
#include <vector>


#include "EView.h"
#include "Singleton.h"

// Class originates from my personal (private) project - https://matthiasseys.me/Projects/crea2dengine.html

class EViewHolder : public Singleton<EViewHolder>
{
public:
	template<typename T>
	T* AddView(const std::string& name = "", const ImGuiWindowFlags& flags = 0)
	{
		T* pView = new T();
		
		if(!name.empty()) pView->SetName(name);
		if(flags != 0) pView->SetFlags(flags);

		m_pEViews.push_back(pView);

		return pView;
	}

	void DrawViews()
	{
		std::vector<EView*> closedViews{};
		closedViews.reserve(m_pEViews.size());

		for(EView* pView : m_pEViews)
		{
			if (!pView->IsOpen())
			{
				closedViews.push_back(pView);
				continue;
			}

			ImGui::PushID(pView);
			pView->DrawGUI();
			ImGui::PopID();
		}

		for(EView* pClosedView : closedViews)
			RemoveView(pClosedView);
	}

	void Destroy()
	{
		for(EView* pView : m_pEViews)
			SafeDelete(pView);
	}
	
private:
	std::list<EView*> m_pEViews;

	void RemoveView(EView* pView)
	{
		m_pEViews.remove(pView);
		SafeDelete(pView);
	}
};