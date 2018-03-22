#include "BowDIMouseDevice.h"
#include "BowLogger.h"

#include "BowVector2.h"
#include "BowVector3.h"
#include "BowDITypeConverter.h"

#define BUFFER_SIZE 16

namespace bow {
	
	DIMouseDevice::DIMouseDevice(IDirectInput8 *directInput, HWND windowHandle) : DIInputDeviceBase(directInput, windowHandle)
	{

		m_Event = nullptr;
		memset(m_Pressed, false, sizeof(m_Pressed));

		m_x = 0;
		m_y = 0;

		m_relativeX = 0;
		m_relativeY = 0;

		m_shouldCage = false;
		m_cageBounds.left = m_cageBounds.top = 0;
		m_cageBounds.right = m_cageBounds.bottom = 999999;
	}

	DIMouseDevice::~DIMouseDevice()
	{
	}

	bool DIMouseDevice::Initialize(void)
	{
		memset(m_Pressed, false, sizeof(m_Pressed));
		m_x = m_y = 0;

		if (!DIInputDeviceBase::Initialize(GUID_SysMouse, &c_dfDIMouse))
		{
			return false;
		}

		if (!(m_Event = CreateEvent(NULL, FALSE, FALSE, NULL)))
		{
			return false;
		}

		if (FAILED(m_pDirectInputDevice->SetEventNotification(m_Event)))
		{
			return false;
		}

		// Buffer
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj		= 0;
		dipdw.diph.dwHow		= DIPH_DEVICE;
		dipdw.dwData			= BUFFER_SIZE;

		if (FAILED(m_pDirectInputDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
		{
			return false;
		}

		m_pDirectInputDevice->Acquire();
		return true;
	}

	bool DIMouseDevice::VUpdate(void)
	{
		DIDEVICEOBJECTDATA od[BUFFER_SIZE];
		DWORD dwNumElem = BUFFER_SIZE;

		memset(&od, 0, sizeof(od));

		if (FAILED(GetData(InputDeviceType::Mouse, &od[0], &dwNumElem)))
		{
			return false;
		}

		POINT point;
		if (GetCursorPos(&point))
		{
			m_x = point.x;
			m_y = point.y;

			if (m_shouldCage)
			{
				if (m_x < m_cageBounds.left)
				{
					m_x = m_cageBounds.left;
				}
				else if (m_x > m_cageBounds.right)
				{
					m_x = m_cageBounds.right;
				}

				if (m_y < m_cageBounds.top)
				{
					m_y = m_cageBounds.top;
				}
				else if (m_y > m_cageBounds.bottom)
				{
					m_y = m_cageBounds.bottom;
				}

				SetCursorPos(m_x, m_y);
			}
		}
		m_relativeX = m_relativeY = m_relativeScrollWheel = 0;
		for (DWORD i = 0; i < dwNumElem; i++)
		{
			switch (od[i].dwOfs)
			{
			case DIMOFS_X:
				m_relativeX += od[i].dwData;
				break;

			case DIMOFS_Y:
				m_relativeY += od[i].dwData;
				break;

			case DIMOFS_Z:
				m_relativeScrollWheel += od[i].dwData;
				break;
					
			case DIMOFS_BUTTON0:
				m_Pressed[0] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON1:
				m_Pressed[1] = (od[i].dwData & 0x80) ? true : false;					
				break;

			case DIMOFS_BUTTON2:
				m_Pressed[2] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON3:
				m_Pressed[3] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON4:
				m_Pressed[4] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON5:
				m_Pressed[5] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON6:
				m_Pressed[6] = (od[i].dwData & 0x80) ? true : false;
				break;

			case DIMOFS_BUTTON7:
				m_Pressed[7] = (od[i].dwData & 0x80) ? true : false;
				break;
			}
		}

		return true;
	}

	bool DIMouseDevice::VIsPressed(MouseButton btnID) const
	{
		return m_Pressed[DITypeConverter::To(btnID)];
	}

	Vector3<long> DIMouseDevice::VGetRelativePosition() const
	{
		return Vector3<long>(m_relativeX, m_relativeY, m_relativeScrollWheel);
	}

	Vector2<long> DIMouseDevice::VGetAbsolutePosition() const
	{
		return Vector2<long>(m_x, m_y);
	}

	Vector2<long> DIMouseDevice::VGetAbsolutePositionInsideWindow() const
	{
		POINT point;
		if (GetCursorPos(&point))
		{
			if (ScreenToClient(m_windowHandle, &point))
			{
				return Vector2<long>(point.x, point.y);
			}
		}

		return Vector2<long>(0, 0);
	}

	bool DIMouseDevice::VSetCursorPosition(int x, int y)
	{
		if (SetCursorPos(x, y))
		{
			m_x = x;
			m_y = y;
			return true;
		}
		return false;
	}

	void DIMouseDevice::VCageMouse(bool cage)
	{
		m_shouldCage = cage;
	}

	void DIMouseDevice::VSetCage(long left, long top, long right, long bottom)
	{
		m_cageBounds.left = left;
		m_cageBounds.top = top;
		m_cageBounds.right = right;
		m_cageBounds.bottom = bottom;
	}

}
