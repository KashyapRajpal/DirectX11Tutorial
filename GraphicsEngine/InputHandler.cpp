#include "InputHandler.h"


InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
}

void InputHandler::Init()
{
	for (int i = 0; i < 256; i++)
	{
		m_inputKeys[i] = false;
	}
}

void InputHandler::Release()
{

}

bool InputHandler::IsKeyDown(unsigned int key)
{
	if (key > 255)
		return false;

	return m_inputKeys[key];
}

void InputHandler::SetKeyDown(unsigned int key)
{
	if (key > 255)
		return;

	m_inputKeys[key] = true;
}

void InputHandler::SetKeyUp(unsigned int key)
{
	if (key > 255)
		return;

	m_inputKeys[key] = false;
}