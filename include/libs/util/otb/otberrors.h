#ifndef PR_OTBERRORS_H
#define PR_OTBERRORS_H

#include <exception>

struct OTBLoadError : std::exception
{
	const char* what() const noexcept override = 0;
};

struct InvalidOTBFormat final : OTBLoadError
{
	const char* what() const noexcept override { return "Invalid OTB file format"; }
};

#endif
