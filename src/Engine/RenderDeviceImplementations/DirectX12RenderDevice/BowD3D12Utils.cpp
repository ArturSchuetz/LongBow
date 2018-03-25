#include "BowD3D12RenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "IBowTexture2D.h"

#include "BowD3D12GraphicsWindow.h"

// for _com_error
#include <comdef.h>

namespace bow {

	std::string widestring2string(const std::wstring& wstr)
	{
		if (wstr.empty())
			return std::string();

		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

		return strTo;
	}

	std::wstring string2widestring(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	std::string toErrorString(HRESULT hresult)
	{
		_com_error err(hresult);
		LPCTSTR errMsg = err.ErrorMessage();
		return std::string(widestring2string(errMsg));
	}
}
