#pragma once
#include "BowPrerequisites.h"
#include "BowInputPredeclares.h"

#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Input {

		enum class API : char
		{
			DirectInput
		};

		//! \brief InputDeviceManager is a singleton and creates input devices. 
		class InputDeviceManager
		{
		public:
			~InputDeviceManager(void);

			static InputDeviceManager& GetInstance();

			KeyboardPtr CreateKeyboardObject(Renderer::GraphicsWindowPtr window);
			MousePtr CreateMouseObject(Renderer::GraphicsWindowPtr window);

		protected:
			InputDeviceManager(){}

		private:
			InputDevicePtr GetOrCreateDevice(API api);
			void ReleaseDevice(API api);

			InputDeviceManager(const InputDeviceManager&){}; //!< You shall not copy
			InputDeviceManager& operator=(const InputDeviceManager&) { return *this; }
		};
	}
}