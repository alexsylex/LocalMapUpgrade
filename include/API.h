#pragma once

namespace LMU
{
	struct PixelShaderProperty
	{
		enum class Shape
		{
			kSquared,
			kRound
		};

		enum class Style
		{
			kBlackNWhite,
			kColor
		};
	};

	namespace API
	{
		struct Message
		{
			enum Type : std::uint32_t
			{
				kPixelShaderPropertiesHook
			};
		};

		struct PixelShaderPropertiesHookMessage : Message
		{
			static constexpr inline Type type = Type::kPixelShaderPropertiesHook;

			void (*SetPixelShaderProperties)(PixelShaderProperty::Shape a_shape, PixelShaderProperty::Style a_style) = {};
			void (*GetPixelShaderProperties)(PixelShaderProperty::Shape& a_shape, PixelShaderProperty::Style& a_style) = {};
		};

		template <typename T>
		concept valid_message = std::is_base_of_v<Message, T>;

		template <typename MessageT>
			requires valid_message<MessageT> inline const MessageT* TranslateAs(SKSE::MessagingInterface::Message* a_msg)
		{
			if constexpr (std::is_same_v<Message, MessageT>) {
				return static_cast<Message*>(a_msg->data);
			}
			else {
				if (a_msg->type == MessageT::type && a_msg->dataLen == sizeof(MessageT)) {
					return static_cast<MessageT*>(a_msg->data);
				}

				return nullptr;
			}
		}
	}
}