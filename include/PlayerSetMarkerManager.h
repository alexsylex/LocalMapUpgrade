#pragma once

#ifdef MessageBox
#define MessageBox_BAK MessageBox
#undef MessageBox
#endif

namespace LMU
{
	class PlayerSetMarkerManager
	{
		struct MessageBox
		{
			struct Callback : RE::IMessageBoxCallback
			{
				void Run(Message a_optionIndex) final;

				void SetData(RE::LocalMapMenu* a_localMapMenu, float a_wndPointX, float a_wndPointY)
				{
					localMapMenu = a_localMapMenu;
					wndPointX = a_wndPointX;
					wndPointY = a_wndPointY;
				}

				RE::LocalMapMenu* localMapMenu = nullptr;
				float wndPointX = 0.0F;
				float wndPointY = 0.0F;
			};

			MessageBox()
			{
				options.push_back(RE::GameSettingCollection::GetSingleton()->GetSetting("sMoveMarker")->data.s);
				options.push_back(RE::GameSettingCollection::GetSingleton()->GetSetting("sLeaveMarker")->data.s);
				options.push_back(RE::GameSettingCollection::GetSingleton()->GetSetting("sRemoveMarker")->data.s);
			}

			RE::BSString title = RE::GameSettingCollection::GetSingleton()->GetSetting("sMoveMarkerQuestion")->data.s;
			RE::BSTArray<RE::BSString> options;
			RE::BSTSmartPointer<Callback> callback = RE::make_smart<Callback>();
		};

	public:
		static PlayerSetMarkerManager* GetSingleton()
		{
			static PlayerSetMarkerManager singleton;
			return &singleton;
		}

		bool CanPlaceMarker() const { return allowPlaceMarker; }
		void AllowPlaceMarker() { allowPlaceMarker = true; }

		void PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_wndPointX, float a_wndPointY);

	private:
		MessageBox messageBox;
		bool allowPlaceMarker = true;
	};
}

#ifdef MessageBox_BAK
#define MessageBox MessageBox_BAK
#undef MessageBox_BAK
#endif
