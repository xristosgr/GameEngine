#pragma once
#include<foundation/PxErrorCallback.h>
#include<PxPhysXConfig.h>


#if !PX_DOXYGEN
//namespace physx
//{
#endif

	/**
	\brief default implementation of the error callback

	This class is provided in order to enable the SDK to be started with the minimum of user code. Typically an application
	will use its own error callback, and log the error to file or otherwise make it visible. Warnings and error messages from
	the SDK are usually indicative that changes are required in order for PhysX to function correctly, and should not be ignored.
	*/

class PxDefaultErrorCallback : public physx::PxErrorCallback
{
public:
	PxDefaultErrorCallback();
	~PxDefaultErrorCallback();

	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
};

#if !PX_DOXYGEN
//} // namespace physx
#endif
