#pragma once

#include "RE/B/BSTSmartPointer.h"
#include "RE/N/NiCamera.h"
#include "RE/N/NiPoint3.h"
#include "RE/N/NiSmartPointer.h"
#include "RE/T/TESCamera.h"
#include "RE/T/TESCameraState.h"

namespace RE
{
	class LocalMapCamera : public TESCamera
	{
	public:
		inline static constexpr auto RTTI = RTTI_LocalMapCamera;

		class DefaultState : public TESCameraState
		{
		public:
			inline static constexpr auto RTTI = RTTI_LocalMapCamera__DefaultState;
			inline static constexpr auto VTABLE = VTABLE_LocalMapCamera__DefaultState;

			~DefaultState() override;  // 00

			// add
			void Begin() override;												 // 01
			void Update(BSTSmartPointer<TESCameraState>& a_nextState) override;	 // 03

			// members
			NiPoint3 initialPosition;	 // 20
			NiPoint3 translation;		 // 2C
			float zoom;					 // 38
			float minFrustumHalfWidth;	 // 3C
			float minFrustumHalfHeight;	 // 40
			std::uint32_t pad44;		 // 44
		};
		static_assert(sizeof(DefaultState) == 0x48);

		~LocalMapCamera() override;	 // 00

		void SetAreaBounds(NiPoint3& a_maxExtent, NiPoint3& a_minExtent);
		void SetDefaultStateInitialPosition(NiPoint3& a_position);
		void SetDefaultStateMinFrustumDimensions(float a_width, float a_height);
		void SetDefaultStateTranslation(float a_x, float a_y, float a_z);
		void SetNorthRotation(float a_northRotation);

		RE::NiPoint2 WorldToScreen(const RE::NiPoint3& a_position) const
		{
			RE::NiPoint2 viewPos;
			float z;

			camera->WorldPtToScreenPt3(camera->GetRuntimeData().worldToCam,
				camera->GetRuntimeData2().port, a_position, viewPos.x, viewPos.y, z, 0.00001F);
			viewPos.y = 1.0F - viewPos.y;

			return viewPos;
		}

		// members
		NiPoint3 maxExtent;							 // 38
		NiPoint3 minExtent;							 // 44
		BSTSmartPointer<DefaultState> defaultState;	 // 50
		NiPointer<NiCamera> camera;					 // 58
		float zRotation;							 // 60
		std::uint32_t pad64;						 // 64

	protected:
		LocalMapCamera* Ctor(float a_zRotation);
	};
	static_assert(sizeof(LocalMapCamera) == 0x68);
}
