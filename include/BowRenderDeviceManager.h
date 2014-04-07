#pragma once
#include "BowPrerequisites.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Bow {
	namespace Renderer {

		class IRenderDevice;
		typedef std::shared_ptr<IRenderDevice> RenderDevicePtr;

		enum class API : char
		{
			OpenGL3x,
			Direct3D11
		};

		//! \brief RenderDeviceManager is a singleton and creates devices. 
		class RenderDeviceManager
		{
		protected:
			RenderDeviceManager(){}

		private:
			RenderDeviceManager(const RenderDeviceManager&){}; //!< You shall not copy
			RenderDeviceManager& operator=(const RenderDeviceManager&) { return *this; }

		public:
			~RenderDeviceManager(void);

			static RenderDeviceManager& GetInstance();

			//! \brief Create an Device with an specific API
			//! \param api The API which should be used.
			//! \return shared_pointer of RenderDevice
			RenderDevicePtr GetOrCreateDevice(API api);
			RenderDevicePtr GetOrCreateDevice(API api, HINSTANCE hInstance);
			void ReleaseDevice(API api);
		};



	}
}