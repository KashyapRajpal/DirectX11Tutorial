#pragma once
class InputHandler
{
public:
	InputHandler();
	~InputHandler();
	void Init();
	void Release();

	bool IsKeyDown(unsigned int key);
	void SetKeyUp(unsigned int key);
	void SetKeyDown(unsigned int key);

private:
	bool m_inputKeys[256];
};

