#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	struct ColorMask
	{
	public:

		ColorMask(bool red, bool green, bool blue, bool alpha)
		{
			m_red = red;
			m_green = green;
			m_blue = blue;
			m_alpha = alpha;
		}

		bool GetRed()
		{
			return m_red;
		}

		bool GetGreen()
		{
			return m_green;
		}

		bool GetBlue()
		{
			return m_blue;
		}

		bool GetAlpha()
		{
			return m_alpha;
		}

		bool operator ==(ColorMask other)
		{
			return
				m_red == other.m_red &&
				m_green == other.m_green &&
				m_blue == other.m_blue &&
				m_alpha == other.m_alpha;
		}

		bool operator !=(ColorMask other)
		{
			return
				m_red != other.m_red ||
				m_green != other.m_green ||
				m_blue != other.m_blue ||
				m_alpha != other.m_alpha;
		}

	private:

		bool m_red;
		bool m_green;
		bool m_blue;
		bool m_alpha;
	};

}
