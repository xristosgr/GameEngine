// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2016-2020 NVIDIA Corporation. All rights reserved.


#ifndef NVBLASTTKFAMILY_H
#define NVBLASTTKFAMILY_H

#include "NvBlastTkIdentifiable.h"


// Forward declarations
struct NvBlastFamily;


namespace Nv
{
namespace Blast
{

// Forward declarations
class TkActor;
class TkAsset;
class TkEventListener;


/**
The TkFamily is associated with the TkActor that is instanced from a TkAsset, as well as all descendent TkActors generated
by spliting TkActors within the family.  It encapsulates an NvBlastFamily, and also holds a material which will be used
by default on all TkActors during damage functions.
*/
class TkFamily : public TkIdentifiable
{
public:
	/**
	Access to underlying low-level family.

	\return a pointer to the (const) low-level NvBlastFamily object.
	*/
	virtual const NvBlastFamily*	getFamilyLL() const = 0;

	/**
	Every family has an associated asset (the TkAsset which was instanced to create the first member of the family).

	\return a pointer to the (const) TkAsset object.
	*/
	virtual const TkAsset*			getAsset() const = 0;

	/**
	The number of actors currently in this family.

	\return the number of TkActors that currently exist in this family.
	*/
	virtual uint32_t				getActorCount() const = 0;

	/**
	Retrieve an array of pointers (into the user-supplied buffer) to actors.

	\param[out]	buffer		A user-supplied array of TkActor pointers.
	\param[in]	bufferSize	The number of elements available to write into buffer.
	\param[in]	indexStart	The starting index of the actor.

	\return the number of TkActor pointers written to the buffer.
	*/
	virtual uint32_t				getActors(TkActor** buffer, uint32_t bufferSize, uint32_t indexStart = 0) const = 0;

	/**
	Add a user implementation of TkEventListener to this family's list of listeners.  These listeners will receive
	all split and fracture events generated by TkActor objects in this family.  They will also receive joint update events
	when TkJoint objects are updated that are (or were) associated with a TkActor in this family.

	\param[in]	l			The event listener to add.
	*/
	virtual void					addListener(TkEventListener& l) = 0;

	/**
	Remove a TkEventReciever from this family's list of listeners.

	\param[in]	l			The event listener to remove.
	*/
	virtual void					removeListener(TkEventListener& l) = 0;

	/**
	This function applies fracture buffers on relevant actors (actor which contains corresponding bond/chunk) in family.

	\param[in]	commands	The fracture commands to process.
	*/
	virtual void					applyFracture(const NvBlastFractureBuffers* commands) = 0;

	/**
	A function to reinitialize this family with new family. The Family must be created from the same low-level asset, but can be 
	in any other state.  As a result split events (TkEvent::Split) will be dispatched reflecting the resulting changes (created and removed actors)
	Afterwards the family will contain a copy of the new family and all actors' low-level actor pointers will be updated.

	\param[in] newFamily		The NvBlastFamily to use to reinitialize this family.
	\param[in] group			The group for new actors to be placed in.
	*/
	virtual void					reinitialize(const NvBlastFamily* newFamily, TkGroup* group = nullptr) = 0;
};

} // namespace Blast
} // namespace Nv


#endif // ifndef NVBLASTTKFAMILY_H
