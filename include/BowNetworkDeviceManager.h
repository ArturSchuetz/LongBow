#pragma once
#include "BowPrerequisites.h"
#include "BowNetworkPredeclares.h"

namespace Bow {
	namespace Network {

		//! \brief RenderDeviceManager is a singleton and creates devices. 
		class NetworkDeviceManager
		{
		public:
			~NetworkDeviceManager(void);

			static NetworkDeviceManager& GetInstance();

			//! \brief Create an Device with an specific API
			//! \param api The API which should be used.
			//! \return shared_pointer of RenderDevice
			NetworkDevicePtr GetOrCreateDevice();
			void ReleaseDevice();

		protected:
			NetworkDeviceManager(){}

		private:
			NetworkDeviceManager(const NetworkDeviceManager&){}; //!< You shall not copy
			NetworkDeviceManager& operator=(const NetworkDeviceManager&) { return *this; }
		};
	}
}