//
//  BDBoxTrigger.h
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and corona.
//

#ifndef __Boardeerlands_BOXTRIGGER_H_
#define __Boardeerlands_BOXTRIGGER_H_

#include <Rayne.h>
#include <RNOpenALWorld.h>

namespace BD
{
	class World;
	class Box;
	class BoxTrigger : public RN::SceneNode
	{
	public:
		BoxTrigger();
		~BoxTrigger();

		void RegisterBox(Box *box);

		bool GetIsActive() const { return _isActive; }
		
		void Update(float delta) override;

	private:
		RN::AABB _boundingBox;
		RN::Array *_boxes;

		bool _isActive;

		RNDeclareMeta(BoxTrigger)
	};
}

#endif /* defined(__Boardeerlands_BOXTRIGGER_H_) */
