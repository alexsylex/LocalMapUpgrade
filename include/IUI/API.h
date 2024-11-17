#pragma once

namespace RE
{
	class GFxMovie;
	class GFxMovieDef;
	class GFxSpriteDef;
	class GFxValue;
	class IMenu;
}

namespace IUI::API
{
	struct Message
	{
		enum Type : std::uint32_t
		{
			kStartLoadInstances,
			kPreReplaceInstance,
			kPostPatchInstance,
			kAbortPatchInstance,
			kFinishLoadInstances,
			kPostInitExtensions
		};

		RE::IMenu* menu;
		RE::GFxMovie* movie;
	};

	struct StartLoadInstancesMessage : Message
	{
		static constexpr inline Type type = Type::kStartLoadInstances;
	};

	struct PreReplaceInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kPreReplaceInstance;

		RE::GFxValue& originalInstance;
	};

	struct PostPatchInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kPostPatchInstance;

		RE::GFxValue& newInstance;
		RE::GFxMovieDef* newInstanceMovieDef;
		RE::GFxSpriteDef* newInstanceSpriteDef;
	};

	struct AbortPatchInstanceMessage : Message
	{
		static constexpr inline Type type = Type::kAbortPatchInstance;

		RE::GFxValue& originalValue;
	};

	struct FinishLoadInstancesMessage : Message
	{
		static constexpr inline Type type = Type::kFinishLoadInstances;

		int loadedCount;
	};

	struct PostInitExtensionsMessage : Message
	{
		static constexpr inline Type type = Type::kPostInitExtensions;
	};

	template <typename T>
	concept valid_message = std::is_base_of_v<Message, T>;

	template <typename MessageT> requires valid_message<MessageT>
	inline const MessageT* TranslateAs(SKSE::MessagingInterface::Message* a_msg)
	{
		if constexpr (std::is_same_v<Message, MessageT>)
		{
			return static_cast<Message*>(a_msg->data);
		}
		else
		{
			if (a_msg->type == MessageT::type && a_msg->dataLen == sizeof(MessageT)) 
			{
				return static_cast<MessageT*>(a_msg->data);
			}

			return nullptr;
		}
	}
}