#pragma once

//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include "vulkanbase/VulkanUtil.h"

//-----------------------------------------------------
// CommandBuffer Class									
//-----------------------------------------------------
class CommandBuffer final
{
public:
	CommandBuffer() = default;				// Constructor
	~CommandBuffer() = default;				// Destructor

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	CommandBuffer(const CommandBuffer& other)					= default;
	CommandBuffer(CommandBuffer&& other) noexcept				= default;
	CommandBuffer& operator=(const CommandBuffer& other)		= default;
	CommandBuffer& operator=(CommandBuffer&& other)	noexcept	= default;

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------



private: 
	//-------------------------------------------------
	// Private member functions								
	//-------------------------------------------------


	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------

	
};

 
