///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_SOCKET_SEND_TO_OPERATION_HPP_INCLUDED
#define CPPCORO_NET_SOCKET_SEND_TO_OPERATION_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_CONFIG_HPP_INCLUDED
#define CPPCORO_CONFIG_HPP_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// Compiler Detection

#if defined(_MSC_VER)
# define CPPCORO_COMPILER_MSVC _MSC_FULL_VER
#else
# define CPPCORO_COMPILER_MSVC 0
#endif

#if defined(__clang__)
# define CPPCORO_COMPILER_CLANG (__clang_major__ * 10000 + \
                                 __clang_minor__ * 100 + \
                                 __clang_patchlevel__)
#else
# define CPPCORO_COMPILER_CLANG 0
#endif

#if defined(__GNUC__)
# define CPPCORO_COMPILER_GCC (__GNUC__ * 10000 + \
                               __GNUC_MINOR__ * 100 + \
                               __GNUC_PATCHLEVEL__)
#else
# define CPPCORO_COMPILER_GCC 0
#endif

/// \def CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
/// Defined to 1 if the compiler supports returning a coroutine_handle from
/// the await_suspend() method as a way of transferring execution
/// to another coroutine with a guaranteed tail-call.
#if CPPCORO_COMPILER_CLANG
# if __clang_major__ >= 7
#  define CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 1
# endif
#endif
#ifndef CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
# define CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 0
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_ASSUME(X) __assume(X)
#else
# define CPPCORO_ASSUME(X)
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_NOINLINE __declspec(noinline)
#elif CPPCORO_COMPILER_CLANG || CPPCORO_COMPILER_GCC
# define CPPCORO_NOINLINE __attribute__((noinline))
#else
# define CPPCORO_NOINLINE
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_FORCE_INLINE __forceinline
#elif CPPCORO_COMPILER_CLANG
# define CPPCORO_FORCE_INLINE __attribute__((always_inline))
#else
# define CPPCORO_FORCE_INLINE inline
#endif

/////////////////////////////////////////////////////////////////////////////
// OS Detection

/// \def CPPCORO_OS_WINNT
/// Defined to non-zero if the target platform is a WindowsNT variant.
/// 0x0500 - Windows 2000
/// 0x0501 - Windows XP/Server 2003
/// 0x0502 - Windows XP SP2/Server 2003 SP1
/// 0x0600 - Windows Vista/Server 2008
/// 0x0601 - Windows 7
/// 0x0602 - Windows 8
/// 0x0603 - Windows 8.1
/// 0x0A00 - Windows 10
#if defined(_WIN32_WINNT) || defined(_WIN32)
# if !defined(_WIN32_WINNT)
// Default to targeting Windows 10 if not defined.
#  define _WIN32_WINNT 0x0A00
# endif
# define CPPCORO_OS_WINNT _WIN32_WINNT
#else
# define CPPCORO_OS_WINNT 0
#endif

#if defined(__linux__)
# define CPPCORO_OS_LINUX 1
#else
# define CPPCORO_OS_LINUX 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CPU Detection

/// \def CPPCORO_CPU_X86
/// Defined to 1 if target CPU is of x86 family.
#if CPPCORO_COMPILER_MSVC
# if defined(_M_IX86)
#  define CPPCORO_CPU_X86 1
# endif
#elif CPPCORO_COMPILER_GCC || CPPCORO_COMPILER_CLANG
# if defined(__i386__)
#  define CPPCORO_CPU_X86 1
# endif
#endif
#if !defined(CPPCORO_CPU_X86)
# define CPPCORO_CPU_X86 0
#endif

/// \def CPPCORO_CPU_X64
/// Defined to 1 if the target CPU is x64 family.
#if CPPCORO_COMPILER_MSVC
# if defined(_M_X64)
#  define CPPCORO_CPU_X64 1
# endif
#elif CPPCORO_COMPILER_GCC || CPPCORO_COMPILER_CLANG
# if defined(__x86_64__)
#  define CPPCORO_CPU_X64 1
# endif
#endif
#if !defined(CPPCORO_CPU_X64)
# define CPPCORO_CPU_X64 0
#endif

/// \def CPPCORO_CPU_32BIT
/// Defined if compiling for a 32-bit CPU architecture.
#if CPPCORO_CPU_X86
# define CPPCORO_CPU_32BIT 1
#else
# define CPPCORO_CPU_32BIT 0
#endif

/// \def CPPCORO_CPU_64BIT
/// Defined if compiling for a 64-bit CPU architecture.
#if CPPCORO_CPU_X64
# define CPPCORO_CPU_64BIT 1
#else
# define CPPCORO_CPU_64BIT 0
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_CPU_CACHE_LINE std::hardware_destructive_interference_size
#else
// On most architectures we can assume a 64-byte cache line.
# define CPPCORO_CPU_CACHE_LINE 64
#endif

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_CANCELLATION_TOKEN_HPP_INCLUDED
#define CPPCORO_CANCELLATION_TOKEN_HPP_INCLUDED

namespace cppcoro
{
	class cancellation_source;
	class cancellation_registration;

	namespace detail
	{
		class cancellation_state;
	}

	class cancellation_token
	{
	public:

		/// Construct to a cancellation token that can't be cancelled.
		cancellation_token() noexcept;

		/// Copy another cancellation token.
		///
		/// New token will refer to the same underlying state.
		cancellation_token(const cancellation_token& other) noexcept;

		cancellation_token(cancellation_token&& other) noexcept;

		~cancellation_token();

		cancellation_token& operator=(const cancellation_token& other) noexcept;

		cancellation_token& operator=(cancellation_token&& other) noexcept;

		void swap(cancellation_token& other) noexcept;

		/// Query if it is possible that this operation will be cancelled
		/// or not.
		///
		/// Cancellable operations may be able to take more efficient code-paths
		/// if they don't need to handle cancellation requests.
		bool can_be_cancelled() const noexcept;

		/// Query if some thread has requested cancellation on an associated
		/// cancellation_source object.
		bool is_cancellation_requested() const noexcept;

		/// Throws cppcoro::operation_cancelled exception if cancellation
		/// has been requested for the associated operation.
		void throw_if_cancellation_requested() const;

	private:

		friend class cancellation_source;
		friend class cancellation_registration;

		cancellation_token(detail::cancellation_state* state) noexcept;

		detail::cancellation_state* m_state;

	};

	inline void swap(cancellation_token& a, cancellation_token& b) noexcept
	{
		a.swap(b);
	}
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IP_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IP_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IP_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IP_ADDRESS_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address
	{
		using bytes_t = std::uint8_t[4];

	public:

		constexpr ipv4_address()
			: m_bytes{ 0, 0, 0, 0 }
		{}

		explicit constexpr ipv4_address(std::uint32_t integer)
			: m_bytes{
			static_cast<std::uint8_t>(integer >> 24),
			static_cast<std::uint8_t>(integer >> 16),
			static_cast<std::uint8_t>(integer >> 8),
			static_cast<std::uint8_t>(integer) }
		{}

		explicit constexpr ipv4_address(const std::uint8_t(&bytes)[4])
			: m_bytes{ bytes[0], bytes[1], bytes[2], bytes[3] }
		{}

		explicit constexpr ipv4_address(
			std::uint8_t b0,
			std::uint8_t b1,
			std::uint8_t b2,
			std::uint8_t b3)
			: m_bytes{ b0, b1, b2, b3 }
		{}

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint32_t to_integer() const
		{
			return
				std::uint32_t(m_bytes[0]) << 24 |
				std::uint32_t(m_bytes[1]) << 16 |
				std::uint32_t(m_bytes[2]) << 8 |
				std::uint32_t(m_bytes[3]);
		}

		static constexpr ipv4_address loopback()
		{
			return ipv4_address(127, 0, 0, 1);
		}

		constexpr bool is_loopback() const
		{
			return m_bytes[0] == 127;
		}

		constexpr bool is_private_network() const
		{
			return m_bytes[0] == 10 ||
				(m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 0x10) ||
				(m_bytes[0] == 192 && m_bytes[2] == 168);
		}

		constexpr bool operator==(ipv4_address other) const
		{
			return
				m_bytes[0] == other.m_bytes[0] &&
				m_bytes[1] == other.m_bytes[1] &&
				m_bytes[2] == other.m_bytes[2] &&
				m_bytes[3] == other.m_bytes[3];
		}

		constexpr bool operator!=(ipv4_address other) const
		{
			return !(*this == other);
		}

		constexpr bool operator<(ipv4_address other) const
		{
			return to_integer() < other.to_integer();
		}

		constexpr bool operator>(ipv4_address other) const
		{
			return other < *this;
		}

		constexpr bool operator<=(ipv4_address other) const
		{
			return !(other < *this);
		}

		constexpr bool operator>=(ipv4_address other) const
		{
			return !(*this < other);
		}

		/// Parse a string representation of an IP address.
		///
		/// Parses strings of the form:
		/// - "num.num.num.num" where num is an integer in range [0, 255].
		/// - A single integer value in range [0, 2^32).
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv4_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to dotted decimal notation.
		///
		/// eg. "12.67.190.23"
		std::string to_string() const;

	private:

		alignas(std::uint32_t) std::uint8_t m_bytes[4];

	};
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address;

	class ipv6_address
	{
		using bytes_t = std::uint8_t[16];

	public:

		constexpr ipv6_address();

		explicit constexpr ipv6_address(
			std::uint64_t subnetPrefix,
			std::uint64_t interfaceIdentifier);

		constexpr ipv6_address(
			std::uint16_t part0,
			std::uint16_t part1,
			std::uint16_t part2,
			std::uint16_t part3,
			std::uint16_t part4,
			std::uint16_t part5,
			std::uint16_t part6,
			std::uint16_t part7);

		explicit constexpr ipv6_address(
			const std::uint16_t(&parts)[8]);

		explicit constexpr ipv6_address(
			const std::uint8_t(&bytes)[16]);

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint64_t subnet_prefix() const;

		constexpr std::uint64_t interface_identifier() const;

		/// Get the IPv6 unspedified address :: (all zeroes).
		static constexpr ipv6_address unspecified();

		/// Get the IPv6 loopback address ::1.
		static constexpr ipv6_address loopback();

		/// Parse a string representation of an IPv6 address.
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv6_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to contracted string form.
		///
		/// Address is broken up into 16-bit parts, with each part represended in 1-4
		/// lower-case hexadecimal with leading zeroes omitted. Parts are separated
		/// by separated by a ':'. The longest contiguous run of zero parts is contracted
		/// to "::".
		///
		/// For example:
		/// ipv6_address::unspecified() -> "::"
		/// ipv6_address::loopback() -> "::1"
		/// ipv6_address(0x0011223344556677, 0x8899aabbccddeeff) ->
		///   "11:2233:4455:6677:8899:aabb:ccdd:eeff"
		/// ipv6_address(0x0102030400000000, 0x003fc447ab991011) ->
		///   "102:304::3f:c447:ab99:1011"
		std::string to_string() const;

		constexpr bool operator==(const ipv6_address& other) const;
		constexpr bool operator!=(const ipv6_address& other) const;
		constexpr bool operator<(const ipv6_address& other) const;
		constexpr bool operator>(const ipv6_address& other) const;
		constexpr bool operator<=(const ipv6_address& other) const;
		constexpr bool operator>=(const ipv6_address& other) const;

	private:

		alignas(std::uint64_t) std::uint8_t m_bytes[16];

	};

	constexpr ipv6_address::ipv6_address()
		: m_bytes{
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0 }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint64_t subnetPrefix,
		std::uint64_t interfaceIdentifier)
		: m_bytes{
			static_cast<std::uint8_t>(subnetPrefix >> 56),
			static_cast<std::uint8_t>(subnetPrefix >> 48),
			static_cast<std::uint8_t>(subnetPrefix >> 40),
			static_cast<std::uint8_t>(subnetPrefix >> 32),
			static_cast<std::uint8_t>(subnetPrefix >> 24),
			static_cast<std::uint8_t>(subnetPrefix >> 16),
			static_cast<std::uint8_t>(subnetPrefix >> 8),
			static_cast<std::uint8_t>(subnetPrefix),
			static_cast<std::uint8_t>(interfaceIdentifier >> 56),
			static_cast<std::uint8_t>(interfaceIdentifier >> 48),
			static_cast<std::uint8_t>(interfaceIdentifier >> 40),
			static_cast<std::uint8_t>(interfaceIdentifier >> 32),
			static_cast<std::uint8_t>(interfaceIdentifier >> 24),
			static_cast<std::uint8_t>(interfaceIdentifier >> 16),
			static_cast<std::uint8_t>(interfaceIdentifier >> 8),
			static_cast<std::uint8_t>(interfaceIdentifier) }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint16_t part0,
		std::uint16_t part1,
		std::uint16_t part2,
		std::uint16_t part3,
		std::uint16_t part4,
		std::uint16_t part5,
		std::uint16_t part6,
		std::uint16_t part7)
		: m_bytes{
			static_cast<std::uint8_t>(part0 >> 8),
			static_cast<std::uint8_t>(part0),
			static_cast<std::uint8_t>(part1 >> 8),
			static_cast<std::uint8_t>(part1),
			static_cast<std::uint8_t>(part2 >> 8),
			static_cast<std::uint8_t>(part2),
			static_cast<std::uint8_t>(part3 >> 8),
			static_cast<std::uint8_t>(part3),
			static_cast<std::uint8_t>(part4 >> 8),
			static_cast<std::uint8_t>(part4),
			static_cast<std::uint8_t>(part5 >> 8),
			static_cast<std::uint8_t>(part5),
			static_cast<std::uint8_t>(part6 >> 8),
			static_cast<std::uint8_t>(part6),
			static_cast<std::uint8_t>(part7 >> 8),
			static_cast<std::uint8_t>(part7) }
	{}

	constexpr ipv6_address::ipv6_address(
		const std::uint16_t(&parts)[8])
		: ipv6_address(
			parts[0], parts[1], parts[2], parts[3],
			parts[4], parts[5], parts[6], parts[7])
	{}

	constexpr ipv6_address::ipv6_address(const std::uint8_t(&bytes)[16])
		: m_bytes{
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11],
			bytes[12], bytes[13], bytes[14], bytes[15] }
	{}

	constexpr std::uint64_t ipv6_address::subnet_prefix() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[0]) << 56 |
			static_cast<std::uint64_t>(m_bytes[1]) << 48 |
			static_cast<std::uint64_t>(m_bytes[2]) << 40 |
			static_cast<std::uint64_t>(m_bytes[3]) << 32 |
			static_cast<std::uint64_t>(m_bytes[4]) << 24 |
			static_cast<std::uint64_t>(m_bytes[5]) << 16 |
			static_cast<std::uint64_t>(m_bytes[6]) << 8 |
			static_cast<std::uint64_t>(m_bytes[7]);
	}

	constexpr std::uint64_t ipv6_address::interface_identifier() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[8]) << 56 |
			static_cast<std::uint64_t>(m_bytes[9]) << 48 |
			static_cast<std::uint64_t>(m_bytes[10]) << 40 |
			static_cast<std::uint64_t>(m_bytes[11]) << 32 |
			static_cast<std::uint64_t>(m_bytes[12]) << 24 |
			static_cast<std::uint64_t>(m_bytes[13]) << 16 |
			static_cast<std::uint64_t>(m_bytes[14]) << 8 |
			static_cast<std::uint64_t>(m_bytes[15]);
	}

	constexpr ipv6_address ipv6_address::unspecified()
	{
		return ipv6_address{};
	}

	constexpr ipv6_address ipv6_address::loopback()
	{
		return ipv6_address{ 0, 0, 0, 0, 0, 0, 0, 1 };
	}

	constexpr bool ipv6_address::operator==(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i]) return false;
		}
		return true;
	}

	constexpr bool ipv6_address::operator!=(const ipv6_address& other) const
	{
		return !(*this == other);
	}

	constexpr bool ipv6_address::operator<(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i])
				return m_bytes[i] < other.m_bytes[i];
		}

		return false;
	}

	constexpr bool ipv6_address::operator>(const ipv6_address& other) const
	{
		return (other < *this);
	}

	constexpr bool ipv6_address::operator<=(const ipv6_address& other) const
	{
		return !(other < *this);
	}

	constexpr bool ipv6_address::operator>=(const ipv6_address& other) const
	{
		return !(*this < other);
	}
}

#endif

#include <cassert>
#include <optional>
#include <string>

namespace cppcoro
{
	namespace net
	{
		class ip_address
		{
		public:

			// Constructs to IPv4 address 0.0.0.0
			ip_address() noexcept;

			ip_address(ipv4_address address) noexcept;
			ip_address(ipv6_address address) noexcept;

			bool is_ipv4() const noexcept { return m_family == family::ipv4; }
			bool is_ipv6() const noexcept { return m_family == family::ipv6; }

			const ipv4_address& to_ipv4() const;
			const ipv6_address& to_ipv6() const;

			const std::uint8_t* bytes() const noexcept;

			std::string to_string() const;

			static std::optional<ip_address> from_string(std::string_view string) noexcept;

			bool operator==(const ip_address& rhs) const noexcept;
			bool operator!=(const ip_address& rhs) const noexcept;

			//  ipv4_address sorts less than ipv6_address
			bool operator<(const ip_address& rhs) const noexcept;
			bool operator>(const ip_address& rhs) const noexcept;
			bool operator<=(const ip_address& rhs) const noexcept;
			bool operator>=(const ip_address& rhs) const noexcept;

		private:

			enum class family
			{
				ipv4,
				ipv6
			};

			family m_family;

			union
			{
				ipv4_address m_ipv4;
				ipv6_address m_ipv6;
			};

		};

		inline ip_address::ip_address() noexcept
			: m_family(family::ipv4)
			, m_ipv4()
		{}

		inline ip_address::ip_address(ipv4_address address) noexcept
			: m_family(family::ipv4)
			, m_ipv4(address)
		{}

		inline ip_address::ip_address(ipv6_address address) noexcept
			: m_family(family::ipv6)
			, m_ipv6(address)
		{
		}

		inline const ipv4_address& ip_address::to_ipv4() const
		{
			assert(is_ipv4());
			return m_ipv4;
		}

		inline const ipv6_address& ip_address::to_ipv6() const
		{
			assert(is_ipv6());
			return m_ipv6;
		}

		inline const std::uint8_t* ip_address::bytes() const noexcept
		{
			return is_ipv4() ? m_ipv4.bytes() : m_ipv6.bytes();
		}

		inline bool ip_address::operator==(const ip_address& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return rhs.is_ipv4() && m_ipv4 == rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 == rhs.m_ipv6;
			}
		}

		inline bool ip_address::operator!=(const ip_address& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline bool ip_address::operator<(const ip_address& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return !rhs.is_ipv4() || m_ipv4 < rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 < rhs.m_ipv6;
			}
		}

		inline bool ip_address::operator>(const ip_address& rhs) const noexcept
		{
			return rhs < *this;
		}

		inline bool ip_address::operator<=(const ip_address& rhs) const noexcept
		{
			return !(rhs < *this);
		}

		inline bool ip_address::operator>=(const ip_address& rhs) const noexcept
		{
			return !(*this < rhs);
		}
	}
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV4_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address
	{
		using bytes_t = std::uint8_t[4];

	public:

		constexpr ipv4_address()
			: m_bytes{ 0, 0, 0, 0 }
		{}

		explicit constexpr ipv4_address(std::uint32_t integer)
			: m_bytes{
			static_cast<std::uint8_t>(integer >> 24),
			static_cast<std::uint8_t>(integer >> 16),
			static_cast<std::uint8_t>(integer >> 8),
			static_cast<std::uint8_t>(integer) }
		{}

		explicit constexpr ipv4_address(const std::uint8_t(&bytes)[4])
			: m_bytes{ bytes[0], bytes[1], bytes[2], bytes[3] }
		{}

		explicit constexpr ipv4_address(
			std::uint8_t b0,
			std::uint8_t b1,
			std::uint8_t b2,
			std::uint8_t b3)
			: m_bytes{ b0, b1, b2, b3 }
		{}

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint32_t to_integer() const
		{
			return
				std::uint32_t(m_bytes[0]) << 24 |
				std::uint32_t(m_bytes[1]) << 16 |
				std::uint32_t(m_bytes[2]) << 8 |
				std::uint32_t(m_bytes[3]);
		}

		static constexpr ipv4_address loopback()
		{
			return ipv4_address(127, 0, 0, 1);
		}

		constexpr bool is_loopback() const
		{
			return m_bytes[0] == 127;
		}

		constexpr bool is_private_network() const
		{
			return m_bytes[0] == 10 ||
				(m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 0x10) ||
				(m_bytes[0] == 192 && m_bytes[2] == 168);
		}

		constexpr bool operator==(ipv4_address other) const
		{
			return
				m_bytes[0] == other.m_bytes[0] &&
				m_bytes[1] == other.m_bytes[1] &&
				m_bytes[2] == other.m_bytes[2] &&
				m_bytes[3] == other.m_bytes[3];
		}

		constexpr bool operator!=(ipv4_address other) const
		{
			return !(*this == other);
		}

		constexpr bool operator<(ipv4_address other) const
		{
			return to_integer() < other.to_integer();
		}

		constexpr bool operator>(ipv4_address other) const
		{
			return other < *this;
		}

		constexpr bool operator<=(ipv4_address other) const
		{
			return !(other < *this);
		}

		constexpr bool operator>=(ipv4_address other) const
		{
			return !(*this < other);
		}

		/// Parse a string representation of an IP address.
		///
		/// Parses strings of the form:
		/// - "num.num.num.num" where num is an integer in range [0, 255].
		/// - A single integer value in range [0, 2^32).
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv4_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to dotted decimal notation.
		///
		/// eg. "12.67.190.23"
		std::string to_string() const;

	private:

		alignas(std::uint32_t) std::uint8_t m_bytes[4];

	};
}

#endif

#include <optional>
#include <string>
#include <string_view>

namespace cppcoro
{
	namespace net
	{
		class ipv4_endpoint
		{
		public:

			// Construct to 0.0.0.0:0
			ipv4_endpoint() noexcept
				: m_address()
				, m_port(0)
			{}

			explicit ipv4_endpoint(ipv4_address address, std::uint16_t port = 0) noexcept
				: m_address(address)
				, m_port(port)
			{}

			const ipv4_address& address() const noexcept { return m_address; }

			std::uint16_t port() const noexcept { return m_port; }

			std::string to_string() const;

			static std::optional<ipv4_endpoint> from_string(std::string_view string) noexcept;

		private:

			ipv4_address m_address;
			std::uint16_t m_port;

		};

		inline bool operator==(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return a.address() == b.address() &&
				a.port() == b.port();
		}

		inline bool operator!=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(a == b);
		}

		inline bool operator<(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return a.address() < b.address() ||
				(a.address() == b.address() && a.port() < b.port());
		}

		inline bool operator>(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return b < a;
		}

		inline bool operator<=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(b < a);
		}

		inline bool operator>=(const ipv4_endpoint& a, const ipv4_endpoint& b)
		{
			return !(a < b);
		}
	}
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ENDPOINT_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ENDPOINT_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED
#define CPPCORO_NET_IPV6_ADDRESS_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cppcoro::net
{
	class ipv4_address;

	class ipv6_address
	{
		using bytes_t = std::uint8_t[16];

	public:

		constexpr ipv6_address();

		explicit constexpr ipv6_address(
			std::uint64_t subnetPrefix,
			std::uint64_t interfaceIdentifier);

		constexpr ipv6_address(
			std::uint16_t part0,
			std::uint16_t part1,
			std::uint16_t part2,
			std::uint16_t part3,
			std::uint16_t part4,
			std::uint16_t part5,
			std::uint16_t part6,
			std::uint16_t part7);

		explicit constexpr ipv6_address(
			const std::uint16_t(&parts)[8]);

		explicit constexpr ipv6_address(
			const std::uint8_t(&bytes)[16]);

		constexpr const bytes_t& bytes() const { return m_bytes; }

		constexpr std::uint64_t subnet_prefix() const;

		constexpr std::uint64_t interface_identifier() const;

		/// Get the IPv6 unspedified address :: (all zeroes).
		static constexpr ipv6_address unspecified();

		/// Get the IPv6 loopback address ::1.
		static constexpr ipv6_address loopback();

		/// Parse a string representation of an IPv6 address.
		///
		/// \param string
		/// The string to parse.
		/// Must be in ASCII, UTF-8 or Latin-1 encoding.
		///
		/// \return
		/// The IP address if successful, otherwise std::nullopt if the string
		/// could not be parsed as an IPv4 address.
		static std::optional<ipv6_address> from_string(std::string_view string) noexcept;

		/// Convert the IP address to contracted string form.
		///
		/// Address is broken up into 16-bit parts, with each part represended in 1-4
		/// lower-case hexadecimal with leading zeroes omitted. Parts are separated
		/// by separated by a ':'. The longest contiguous run of zero parts is contracted
		/// to "::".
		///
		/// For example:
		/// ipv6_address::unspecified() -> "::"
		/// ipv6_address::loopback() -> "::1"
		/// ipv6_address(0x0011223344556677, 0x8899aabbccddeeff) ->
		///   "11:2233:4455:6677:8899:aabb:ccdd:eeff"
		/// ipv6_address(0x0102030400000000, 0x003fc447ab991011) ->
		///   "102:304::3f:c447:ab99:1011"
		std::string to_string() const;

		constexpr bool operator==(const ipv6_address& other) const;
		constexpr bool operator!=(const ipv6_address& other) const;
		constexpr bool operator<(const ipv6_address& other) const;
		constexpr bool operator>(const ipv6_address& other) const;
		constexpr bool operator<=(const ipv6_address& other) const;
		constexpr bool operator>=(const ipv6_address& other) const;

	private:

		alignas(std::uint64_t) std::uint8_t m_bytes[16];

	};

	constexpr ipv6_address::ipv6_address()
		: m_bytes{
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0 }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint64_t subnetPrefix,
		std::uint64_t interfaceIdentifier)
		: m_bytes{
			static_cast<std::uint8_t>(subnetPrefix >> 56),
			static_cast<std::uint8_t>(subnetPrefix >> 48),
			static_cast<std::uint8_t>(subnetPrefix >> 40),
			static_cast<std::uint8_t>(subnetPrefix >> 32),
			static_cast<std::uint8_t>(subnetPrefix >> 24),
			static_cast<std::uint8_t>(subnetPrefix >> 16),
			static_cast<std::uint8_t>(subnetPrefix >> 8),
			static_cast<std::uint8_t>(subnetPrefix),
			static_cast<std::uint8_t>(interfaceIdentifier >> 56),
			static_cast<std::uint8_t>(interfaceIdentifier >> 48),
			static_cast<std::uint8_t>(interfaceIdentifier >> 40),
			static_cast<std::uint8_t>(interfaceIdentifier >> 32),
			static_cast<std::uint8_t>(interfaceIdentifier >> 24),
			static_cast<std::uint8_t>(interfaceIdentifier >> 16),
			static_cast<std::uint8_t>(interfaceIdentifier >> 8),
			static_cast<std::uint8_t>(interfaceIdentifier) }
	{}

	constexpr ipv6_address::ipv6_address(
		std::uint16_t part0,
		std::uint16_t part1,
		std::uint16_t part2,
		std::uint16_t part3,
		std::uint16_t part4,
		std::uint16_t part5,
		std::uint16_t part6,
		std::uint16_t part7)
		: m_bytes{
			static_cast<std::uint8_t>(part0 >> 8),
			static_cast<std::uint8_t>(part0),
			static_cast<std::uint8_t>(part1 >> 8),
			static_cast<std::uint8_t>(part1),
			static_cast<std::uint8_t>(part2 >> 8),
			static_cast<std::uint8_t>(part2),
			static_cast<std::uint8_t>(part3 >> 8),
			static_cast<std::uint8_t>(part3),
			static_cast<std::uint8_t>(part4 >> 8),
			static_cast<std::uint8_t>(part4),
			static_cast<std::uint8_t>(part5 >> 8),
			static_cast<std::uint8_t>(part5),
			static_cast<std::uint8_t>(part6 >> 8),
			static_cast<std::uint8_t>(part6),
			static_cast<std::uint8_t>(part7 >> 8),
			static_cast<std::uint8_t>(part7) }
	{}

	constexpr ipv6_address::ipv6_address(
		const std::uint16_t(&parts)[8])
		: ipv6_address(
			parts[0], parts[1], parts[2], parts[3],
			parts[4], parts[5], parts[6], parts[7])
	{}

	constexpr ipv6_address::ipv6_address(const std::uint8_t(&bytes)[16])
		: m_bytes{
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11],
			bytes[12], bytes[13], bytes[14], bytes[15] }
	{}

	constexpr std::uint64_t ipv6_address::subnet_prefix() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[0]) << 56 |
			static_cast<std::uint64_t>(m_bytes[1]) << 48 |
			static_cast<std::uint64_t>(m_bytes[2]) << 40 |
			static_cast<std::uint64_t>(m_bytes[3]) << 32 |
			static_cast<std::uint64_t>(m_bytes[4]) << 24 |
			static_cast<std::uint64_t>(m_bytes[5]) << 16 |
			static_cast<std::uint64_t>(m_bytes[6]) << 8 |
			static_cast<std::uint64_t>(m_bytes[7]);
	}

	constexpr std::uint64_t ipv6_address::interface_identifier() const
	{
		return
			static_cast<std::uint64_t>(m_bytes[8]) << 56 |
			static_cast<std::uint64_t>(m_bytes[9]) << 48 |
			static_cast<std::uint64_t>(m_bytes[10]) << 40 |
			static_cast<std::uint64_t>(m_bytes[11]) << 32 |
			static_cast<std::uint64_t>(m_bytes[12]) << 24 |
			static_cast<std::uint64_t>(m_bytes[13]) << 16 |
			static_cast<std::uint64_t>(m_bytes[14]) << 8 |
			static_cast<std::uint64_t>(m_bytes[15]);
	}

	constexpr ipv6_address ipv6_address::unspecified()
	{
		return ipv6_address{};
	}

	constexpr ipv6_address ipv6_address::loopback()
	{
		return ipv6_address{ 0, 0, 0, 0, 0, 0, 0, 1 };
	}

	constexpr bool ipv6_address::operator==(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i]) return false;
		}
		return true;
	}

	constexpr bool ipv6_address::operator!=(const ipv6_address& other) const
	{
		return !(*this == other);
	}

	constexpr bool ipv6_address::operator<(const ipv6_address& other) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (m_bytes[i] != other.m_bytes[i])
				return m_bytes[i] < other.m_bytes[i];
		}

		return false;
	}

	constexpr bool ipv6_address::operator>(const ipv6_address& other) const
	{
		return (other < *this);
	}

	constexpr bool ipv6_address::operator<=(const ipv6_address& other) const
	{
		return !(other < *this);
	}

	constexpr bool ipv6_address::operator>=(const ipv6_address& other) const
	{
		return !(*this < other);
	}
}

#endif

#include <optional>
#include <string>
#include <string_view>

namespace cppcoro
{
	namespace net
	{
		class ipv6_endpoint
		{
		public:

			// Construct to [::]:0
			ipv6_endpoint() noexcept
				: m_address()
				, m_port(0)
			{}

			explicit ipv6_endpoint(ipv6_address address, std::uint16_t port = 0) noexcept
				: m_address(address)
				, m_port(port)
			{}

			const ipv6_address& address() const noexcept { return m_address; }

			std::uint16_t port() const noexcept { return m_port; }

			std::string to_string() const;

			static std::optional<ipv6_endpoint> from_string(std::string_view string) noexcept;

		private:

			ipv6_address m_address;
			std::uint16_t m_port;

		};

		inline bool operator==(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return a.address() == b.address() &&
				a.port() == b.port();
		}

		inline bool operator!=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(a == b);
		}

		inline bool operator<(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return a.address() < b.address() ||
				(a.address() == b.address() && a.port() < b.port());
		}

		inline bool operator>(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return b < a;
		}

		inline bool operator<=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(b < a);
		}

		inline bool operator>=(const ipv6_endpoint& a, const ipv6_endpoint& b)
		{
			return !(a < b);
		}
	}
}

#endif

#include <cassert>
#include <optional>
#include <string>

namespace cppcoro
{
	namespace net
	{
		class ip_endpoint
		{
		public:

			// Constructs to IPv4 end-point 0.0.0.0:0
			ip_endpoint() noexcept;

			ip_endpoint(ipv4_endpoint endpoint) noexcept;
			ip_endpoint(ipv6_endpoint endpoint) noexcept;

			bool is_ipv4() const noexcept { return m_family == family::ipv4; }
			bool is_ipv6() const noexcept { return m_family == family::ipv6; }

			const ipv4_endpoint& to_ipv4() const;
			const ipv6_endpoint& to_ipv6() const;

			ip_address address() const noexcept;
			std::uint16_t port() const noexcept;

			std::string to_string() const;

			static std::optional<ip_endpoint> from_string(std::string_view string) noexcept;

			bool operator==(const ip_endpoint& rhs) const noexcept;
			bool operator!=(const ip_endpoint& rhs) const noexcept;

			//  ipv4_endpoint sorts less than ipv6_endpoint
			bool operator<(const ip_endpoint& rhs) const noexcept;
			bool operator>(const ip_endpoint& rhs) const noexcept;
			bool operator<=(const ip_endpoint& rhs) const noexcept;
			bool operator>=(const ip_endpoint& rhs) const noexcept;

		private:

			enum class family
			{
				ipv4,
				ipv6
			};

			family m_family;

			union
			{
				ipv4_endpoint m_ipv4;
				ipv6_endpoint m_ipv6;
			};

		};

		inline ip_endpoint::ip_endpoint() noexcept
			: m_family(family::ipv4)
			, m_ipv4()
		{}

		inline ip_endpoint::ip_endpoint(ipv4_endpoint endpoint) noexcept
			: m_family(family::ipv4)
			, m_ipv4(endpoint)
		{}

		inline ip_endpoint::ip_endpoint(ipv6_endpoint endpoint) noexcept
			: m_family(family::ipv6)
			, m_ipv6(endpoint)
		{
		}

		inline const ipv4_endpoint& ip_endpoint::to_ipv4() const
		{
			assert(is_ipv4());
			return m_ipv4;
		}

		inline const ipv6_endpoint& ip_endpoint::to_ipv6() const
		{
			assert(is_ipv6());
			return m_ipv6;
		}

		inline ip_address ip_endpoint::address() const noexcept
		{
			if (is_ipv4())
			{
				return m_ipv4.address();
			}
			else
			{
				return m_ipv6.address();
			}
		}

		inline std::uint16_t ip_endpoint::port() const noexcept
		{
			return is_ipv4() ? m_ipv4.port() : m_ipv6.port();
		}

		inline bool ip_endpoint::operator==(const ip_endpoint& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return rhs.is_ipv4() && m_ipv4 == rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 == rhs.m_ipv6;
			}
		}

		inline bool ip_endpoint::operator!=(const ip_endpoint& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		inline bool ip_endpoint::operator<(const ip_endpoint& rhs) const noexcept
		{
			if (is_ipv4())
			{
				return !rhs.is_ipv4() || m_ipv4 < rhs.m_ipv4;
			}
			else
			{
				return rhs.is_ipv6() && m_ipv6 < rhs.m_ipv6;
			}
		}

		inline bool ip_endpoint::operator>(const ip_endpoint& rhs) const noexcept
		{
			return rhs < *this;
		}

		inline bool ip_endpoint::operator<=(const ip_endpoint& rhs) const noexcept
		{
			return !(rhs < *this);
		}

		inline bool ip_endpoint::operator>=(const ip_endpoint& rhs) const noexcept
		{
			return !(*this < rhs);
		}
	}
}

#endif

#include <cstdint>

#if CPPCORO_OS_WINNT
# include <cppcoro/detail/win32.hpp>
# include <cppcoro/detail/win32_overlapped_operation.hpp>

namespace cppcoro::net
{
	class socket;

	class socket_send_to_operation_impl
	{
	public:

		socket_send_to_operation_impl(
			socket& s,
			const ip_endpoint& destination,
			const void* buffer,
			std::size_t byteCount) noexcept
			: m_socket(s)
			, m_destination(destination)
			, m_buffer(const_cast<void*>(buffer), byteCount)
		{}

		bool try_start(cppcoro::detail::win32_overlapped_operation_base& operation) noexcept;
		void cancel(cppcoro::detail::win32_overlapped_operation_base& operation) noexcept;

	private:

		socket& m_socket;
		ip_endpoint m_destination;
		cppcoro::detail::win32::wsabuf m_buffer;

	};

	class socket_send_to_operation
		: public cppcoro::detail::win32_overlapped_operation<socket_send_to_operation>
	{
	public:

		socket_send_to_operation(
			socket& s,
			const ip_endpoint& destination,
			const void* buffer,
			std::size_t byteCount) noexcept
			: m_impl(s, destination, buffer, byteCount)
		{}

	private:

		friend class cppcoro::detail::win32_overlapped_operation<socket_send_to_operation>;

		bool try_start() noexcept { return m_impl.try_start(*this); }

		socket_send_to_operation_impl m_impl;

	};

	class socket_send_to_operation_cancellable
		: public cppcoro::detail::win32_overlapped_operation_cancellable<socket_send_to_operation_cancellable>
	{
	public:

		socket_send_to_operation_cancellable(
			socket& s,
			const ip_endpoint& destination,
			const void* buffer,
			std::size_t byteCount,
			cancellation_token&& ct) noexcept
			: cppcoro::detail::win32_overlapped_operation_cancellable<socket_send_to_operation_cancellable>(std::move(ct))
			, m_impl(s, destination, buffer, byteCount)
		{}

	private:

		friend class cppcoro::detail::win32_overlapped_operation_cancellable<socket_send_to_operation_cancellable>;

		bool try_start() noexcept { return m_impl.try_start(*this); }
		void cancel() noexcept { return m_impl.cancel(*this); }

		socket_send_to_operation_impl m_impl;

	};

}

#endif // CPPCORO_OS_WINNT

#endif
