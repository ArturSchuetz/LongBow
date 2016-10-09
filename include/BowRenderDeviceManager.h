#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer {

		enum class API : char
		{
			OpenGL3x,
			Vulkan
		};

		//! \brief RenderDeviceManager is a singleton and creates devices. 
		class RenderDeviceManager
		{
		public:
			~RenderDeviceManager(void);

			static RenderDeviceManager& GetInstance();

			//! \brief Create an Device with an specific API
			//! \param api The API which should be used.
			//! \return shared_pointer of RenderDevice
			RenderDevicePtr GetOrCreateDevice(API api);
			void ReleaseDevice(API api);

		protected:
			RenderDeviceManager(){}

		private:
			RenderDeviceManager(const RenderDeviceManager&){}; //!< You shall not copy
			RenderDeviceManager& operator=(const RenderDeviceManager&) { return *this; }
		};
	}
}