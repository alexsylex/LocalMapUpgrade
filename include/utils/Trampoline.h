#pragma once

#if defined(SKSE_SUPPORT_XBYAK)

#include <xbyak/xbyak.h>

#include "SKSE/Trampoline.h"

namespace hooks
{
	template <std::size_t SrcSize>
	class Hook
	{
		static_assert(SrcSize == 5 || SrcSize == 6);

		static constexpr std::size_t getSizeForSrc()
		{
			// Reference: write_5branch() and write_6branch() of Trampoline.h
			if constexpr (SrcSize == 5)
			{
#pragma pack(push, 1)
				// FF /4
				// JMP r/m64
				struct TrampolineAssembly
				{
					// jmp [rip]
					std::uint8_t jmp;	 // 0 - 0xFF
					std::uint8_t modrm;	 // 1 - 0x25
					std::int32_t disp;	 // 2 - 0x00000000
					std::uint64_t addr;	 // 6 - [rip]
				};
#pragma pack(pop)

				return sizeof(TrampolineAssembly);
			}
			else
			{
				return sizeof(std::uintptr_t);
			}
		}

		static constexpr std::size_t getSizeFor(const Xbyak::CodeGenerator& a_dst)
		{
			return getSizeForSrc() + a_dst.getSize();
		}

	public:

		Hook(std::uintptr_t a_src, const Xbyak::CodeGenerator& a_codeGen) :
			size{ getSizeFor(a_codeGen) }, src{ a_src },
			asmCode{ a_codeGen.getCode(), a_codeGen.getCode() + a_codeGen.getSize() }
		{ }

		Hook(std::uintptr_t a_src, std::uintptr_t a_dst) :
			size{ getSizeForSrc() }, src{ a_src }, dst{ a_dst }
		{ }

		std::size_t getSize() const { return size; }

		std::uintptr_t getSrc() const { return src; }

		std::uintptr_t getDst() const { return dst; }

		void createDst(SKSE::Trampoline* a_allocator)
		{
			if (!asmCode.empty())
			{
				void* buffer = a_allocator->allocate(asmCode.size());
				std::memcpy(buffer, asmCode.data(), asmCode.size());
				dst = reinterpret_cast<std::uintptr_t>(buffer);
			}
		}

	private:

		std::size_t size;
		std::uintptr_t src;
		std::uintptr_t dst = reinterpret_cast<std::uintptr_t>(nullptr);
		std::vector<std::uint8_t> asmCode;
	};

	class Trampoline
	{
	public:

		template <std::size_t SrcSize>
		std::uintptr_t write_branch(Hook<SrcSize>& a_hook)
		{
			if (!a_hook.getDst()) {
				a_hook.createDst(inst);
			}

			return inst->write_branch<SrcSize>(a_hook.getSrc(), a_hook.getDst());
		}

		template <std::size_t SrcSize>
		std::uintptr_t write_call(Hook<SrcSize>& a_hook)
		{
			if (!a_hook.getDst()) {
				a_hook.createDst(inst);
			}

			return inst->write_call<SrcSize>(a_hook.getSrc(), a_hook.getDst());
		}

	protected:

		Trampoline(SKSE::Trampoline& a_trampoline) :
			inst{ &a_trampoline }
		{ }

		Trampoline(const std::string_view& a_name) :
			inst{ new SKSE::Trampoline{ a_name } }, deleteOnDestruct{ true }
		{ }

		~Trampoline() { if (deleteOnDestruct) delete inst; }

		SKSE::Trampoline* inst;

	private:

		bool deleteOnDestruct = false;
	};

	class DefaultTrampoline : public Trampoline
	{
	public:

		DefaultTrampoline(std::size_t a_size, bool a_trySKSEReserve = true) :
			Trampoline{ SKSE::GetTrampoline() }
		{
			SKSE::AllocTrampoline(a_size, a_trySKSEReserve);
		}
	};

	class CustomTrampoline : public Trampoline
	{
	public:

		CustomTrampoline(const std::string_view& a_name, void* a_module, std::size_t a_size);
	};

	class SigScanner
	{
	public:

		static std::pair<std::uintptr_t, std::size_t> GetModuleInfo(REX::W32::HMODULE a_moduleHandle);

		template <SKSE::stl::nttp::string str>
		static std::uintptr_t FindPattern(REX::W32::HMODULE a_moduleHandle)
		{
			auto [base, size] = GetModuleInfo(a_moduleHandle);

			std::size_t patternLength = (str.length() + 1) / 3; // 2/3 useful chars (1 space between byte chars)
			auto pattern = REL::make_pattern<str>();

			for (std::size_t offset = 0; offset < size - patternLength; offset++)
			{
				std::uintptr_t addr = base + offset;

				if (pattern.match(addr))
				{
					return addr;
				}
			}

			return reinterpret_cast<std::uintptr_t>(nullptr);
		}
	};
}

#else
#error "Xbyak support needed for Trampoline"
#endif