//
//  BDPlayerTrigger.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_PLAYERTRIGGER_H_
#define __Boardeerlands_PLAYERTRIGGER_H_

#include <Rayne.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class PlayerTrigger : public RN::SceneNode
	{
	public:
		PlayerTrigger();
		~PlayerTrigger();
		
		void Update(float delta) override;

	private:
		RN::AABB _boundingBox;

		RNDeclareMeta(PlayerTrigger)
	};
}

#endif /* defined(__Boardeerlands_PLAYERTRIGGER_H_) */
