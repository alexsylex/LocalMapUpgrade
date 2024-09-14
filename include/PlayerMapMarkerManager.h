#pragma once

namespace LMU
{
	class PlayerMapMarkerManager
	{
	public:
		struct MessageBoxCallback : RE::IMessageBoxCallback
		{
			MessageBoxCallback(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY)
			: localMapMenu{ a_localMapMenu }, cursorPosX{ a_cursorPosX }, cursorPosY{ a_cursorPosY }
			{ }

			void Run(Message a_optionIndex) final;

			RE::LocalMapMenu* localMapMenu;
			float cursorPosX;
			float cursorPosY;
		};

		static PlayerMapMarkerManager* GetSingleton()
		{
			static PlayerMapMarkerManager singleton;

			return &singleton;
		}

		void PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY);

	private:
		const char* const& moveMarkerQuestionTitle = RE::GameSettingCollection::GetSingleton()->GetSetting("sMoveMarkerQuestion")->data.s;
		const char* const& moveMarkerOption = RE::GameSettingCollection::GetSingleton()->GetSetting("sMoveMarker")->data.s;
		const char* const& leaveMarkerOption = RE::GameSettingCollection::GetSingleton()->GetSetting("sLeaveMarker")->data.s;
		const char* const& removeMarkerOption = RE::GameSettingCollection::GetSingleton()->GetSetting("sRemoveMarker")->data.s;
	};
}
