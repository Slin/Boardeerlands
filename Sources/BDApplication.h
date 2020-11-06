//
//  BDApplication.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//

#ifndef __BD_APPLICATION_H_
#define __BD_APPLICATION_H_

#include <Rayne.h>
#include "RNVRApplication.h"

namespace BD
{
	class Application : public RN::VRApplication
	{
	public:
		Application();
		~Application();
		
		void WillFinishLaunching(RN::Kernel *kernel) override;
		void DidFinishLaunching(RN::Kernel *kernel) override;
	};
}


#endif /* __BD_APPLICATION_H_ */
