#include "Stdafx.h"
#include "XingSharp.h"
#include "apiwrap.h"
#include "util.h"
#include "XingSharp.h"

namespace XingSharp {

void ApiWrapper::OnLoginResult(WPARAM wParam, LPARAM lParam)
{
	LPCSTR pszCode = (LPCSTR)wParam;
	LPCSTR pszMsgA = (LPCSTR)lParam;

	if (!strcmp(pszCode, "0000"))
	{
		m_pOwner->OnLoginSuccess();
	}
	else
	{
		std::wstring msgW = KRtoWide(pszMsgA);
		m_pOwner->OnLoginFailed(pszCode, msgW.c_str());
	}
}

} // namespace XingSharp